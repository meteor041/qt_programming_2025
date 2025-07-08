// src/Scenes/BattleScene.cpp

#include <QDebug>
#include <QLineF> // 用于计算距离
#include "BattleScene.h"
#include "../Items/Characters/Link.h" // 假设你的角色是Link
#include "../Items/Maps/Battlefield.h"
#include "../Items/Armors/FlamebreakerArmor.h"
#include "../Items/Maps/platform/Platform.h" // 包含平台基类

// 构造函数：初始化整个战斗场景
BattleScene::BattleScene(QObject *parent) : Scene(parent) {
    setSceneRect(0, 0, 1280, 720);

    // 【核心改动 #1】修复Battlefield的创建
    // Battlefield的构造函数现在需要一个指向场景的指针 (this)
    map = new Battlefield(this);
    addItem(map);
    map->setZValue(-1.0);
    map->scaleToFitScene(this); // 缩放背景图

    // 创建角色
    character = new Link();
    addItem(character);
    character->setScale(1.0);
    character->setZValue(10);
    // 使用新的getSpawnPos，它会基于平台位置来决定出生点
    character->setPos(map->getSpawnPos());

    // 创建备用盔甲
    spareArmor = new FlamebreakerArmor();
    addItem(spareArmor);
    spareArmor->unmount();

    // 【核心改动 #2】修复盔甲的放置位置
    // 我们不能再使用 getFloorHeight()，而是要找到一个平台来放置它
    QPointF armorTargetPos(sceneRect().center().x() + 200, 0); // 设定一个目标水平位置
    Platform* groundForArmor = map->getGroundPlatform(armorTargetPos);
    if (groundForArmor) {
        // 如果找到了平台，将盔甲放在平台表面上方
        spareArmor->setPos(armorTargetPos.x(),
                           groundForArmor->getSurfaceY() - spareArmor->boundingRect().height());
    } else {
        // 如果没找到平台（作为备用方案），放在一个安全的位置
        spareArmor->setPos(sceneRect().center());
    }
}

// 【核心改动 #3】实现物理处理逻辑
void BattleScene::processPhysics() {
    if (!character || !map) {
        return;
    }

    const qreal GRAVITY = 2000.0; // 定义一个重力加速度（像素/秒^2）

    // 1. 对角色施加重力 (除非它在地面上)
    Platform* ground = map->getGroundPlatform(character->pos());

    if (ground == nullptr) { // 如果脚下没有平台，说明在空中
        // 增加向下的速度
        //还没写 character->addVelocity(0, GRAVITY * (deltaTime / 1000.0));
    } else { // 如果脚下有平台
        qreal footY = character->sceneBoundingRect().bottom();
        qreal surfaceY = ground->getSurfaceY();

        // 检查是否正在下落并且已经接触或穿过平台
        if (footY >= surfaceY && character->getVelocity().y() > 0) {
            // 将角色“吸附”在平台表面
            character->setY(surfaceY - character->boundingRect().height());
            // 垂直速度清零，停止下落
        //还没写    character->setVelocity(character->getVelocity().x(), 0);
        }
    }
}


// --- 以下是其他函数的代码，大部分保持不变 ---

// 处理角色移动，现在只负责应用速度，物理修正由processPhysics完成
void BattleScene::processMovement() {
    Scene::processMovement();
    if (character != nullptr) {
        // deltaTime/1000.0 将毫秒转换为秒
        QPointF newPos = character->pos() + character->getVelocity() * (double) deltaTime;

        // 获取边界
        QRectF bounds = sceneRect();
        QRectF charRect = character->boundingRect();

        // 应用边界限制
        newPos.setX(qBound(0.0, newPos.x(), bounds.width() - charRect.width()));
        newPos.setY(qBound(0.0, newPos.y(), bounds.height() - charRect.height()));

        character->setPos(newPos);
    }
}


void BattleScene::processInput() {
    Scene::processInput();
    if (character != nullptr) {
        character->processInput(); // 假设角色内部会根据按键状态设置速度
    }
}

void BattleScene::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_A:
        if (character) character->setLeftDown(true);
        break;
    case Qt::Key_D:
        if (character) character->setRightDown(true);
        break;
    case Qt::Key_J:
        if (character) character->setPickDown(true);
        break;
    // 【建议】增加跳跃键
    case Qt::Key_W:
    case Qt::Key_Space:
        if (character) {
            // 只有在地面上才能跳
            if (map->getGroundPlatform(character->pos()) != nullptr) {
                character->setJumpDown(true);
            }
        }
        break;
    default:
        Scene::keyPressEvent(event);
    }
}

void BattleScene::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_A:
        if (character) character->setLeftDown(false);
        break;
    case Qt::Key_D:
        if (character) character->setRightDown(false);
        break;
    case Qt::Key_J:
        if (character) character->setPickDown(false);
        break;
    case Qt::Key_W:
    case Qt::Key_Space:
        if (character) character->setJumpDown(false);
        break;
    default:
        Scene::keyReleaseEvent(event);
    }
}

void BattleScene::update() {
    // BattleScene的update只需要调用基类的update即可，所有逻辑都在各个process函数里
    Scene::update();
}

void BattleScene::processPicking() {
    Scene::processPicking();
    if (character && character->isPicking()) {
        auto mountable = findNearestUnmountedMountable(character->pos(), 100.);
        if (mountable != nullptr) {
            spareArmor = dynamic_cast<Armor *>(pickupMountable(character, mountable));
        }
    }
}

Mountable *BattleScene::findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold) {
    Mountable *nearest = nullptr;
    qreal minDistance = distance_threshold;
    for (QGraphicsItem *item: items()) {
        if (auto mountable = dynamic_cast<Mountable *>(item)) {
            if (!mountable->isMounted()) {
                qreal distance = QLineF(pos, item->pos()).length();
                if (distance < minDistance) {
                    minDistance = distance;
                    nearest = mountable;
                }
            }
        }
    }
    return nearest;
}

Mountable *BattleScene::pickupMountable(Character *character, Mountable *mountable) {
    if (auto armor = dynamic_cast<Armor *>(mountable)) {
        return character->pickupArmor(armor);
    }
    return nullptr;
}
