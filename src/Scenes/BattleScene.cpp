// src/Scenes/BattleScene.cpp

#include <QDebug>
#include <QLineF>
#include "BattleScene.h"
#include "../Items/Characters/Link.h"
#include "../Items/Maps/Battlefield.h"
#include "../Items/Armors/FlamebreakerArmor.h"
#include "../Items/Maps/platform/Platform.h"
#include "../Items/Weapons/Weapon.h" // [NEW] 需要包含武器

// 构造函数：初始化整个战斗场景
BattleScene::BattleScene(QObject *parent) : Scene(parent) {
    setSceneRect(0, 0, 1280, 720);

    map = new Battlefield(this);
    addItem(map);
    map->setZValue(-1.0);
    map->scaleToFitScene(this);

    // [MODIFIED] 创建两个玩家角色
    character1 = new Link(1); // 玩家1
    addItem(character1);
    character1->setPos(map->getSpawnPos(1)); // 假设getSpawnPos可以接受玩家ID

    character2 = new Link(2); // 玩家2
    addItem(character2);
    character2->setPos(map->getSpawnPos(2));

    // 创建示例物品
    auto* spareArmor = new FlamebreakerArmor();
    addItem(spareArmor);
    spareArmor->unmount();
    worldItems.append(spareArmor);

    // 放置物品
    QPointF armorTargetPos(sceneRect().center().x() + 200, 0);
    Platform* groundForArmor = map->getGroundPlatform(armorTargetPos);
    if (groundForArmor) {
        spareArmor->setPos(armorTargetPos.x(), groundForArmor->getSurfaceY() - spareArmor->boundingRect().height());
    } else {
        spareArmor->setPos(sceneRect().center());
    }
}

// [FIXED] 物理处理逻辑
void BattleScene::processPhysics() {
    const qreal GRAVITY = 2000.0;
    QList<Character*> characters = {character1, character2};

    for (Character* character : characters) {
        if (!character || !character->isEnabled()) continue;

        Platform* ground = map->getGroundPlatform(character->pos());

        if (ground) {
            character->setIsOnGround(true);
            // 修正位置，防止角色掉进平台
            qreal footY = character->sceneBoundingRect().bottom();
            qreal surfaceY = ground->getSurfaceY();
            if (footY > surfaceY && character->getVelocity().y() >= 0) {
                character->setY(surfaceY - character->boundingRect().height());
                character->setVelocity(character->getVelocity().x(), 0);
            }
        } else {
            character->setIsOnGround(false);
            // 在空中，施加重力 (deltaTime在Scene基类中计算)
            character->addVelocity(QPointF(0, GRAVITY * (deltaTime / 1000.0)));
        }
    }
}

// [FIXED] 角色更新逻辑
void BattleScene::processCharacterUpdates() {
    if (character1) character1->update(deltaTime);
    if (character2) character2->update(deltaTime);
}

// [FIXED] 输入逻辑完全在KeyEvents中处理，此函数留空
void BattleScene::processInput() {
    // Intentionally left blank.
}

// [FIXED] 双人键盘控制
void BattleScene::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    // Player 1 Controls (WASD + J/K)
    if (character1) {
        switch (event->key()) {
        case Qt::Key_A: character1->setLeftDown(true); break;
        case Qt::Key_D: character1->setRightDown(true); break;
        case Qt::Key_W: character1->setJumpDown(true); break;
        case Qt::Key_S: character1->setCrouchDown(true); break;
        case Qt::Key_J: character1->setFireDown(true); break;
        default: break;
        }
    }

    // Player 2 Controls (Arrow Keys + Numpad 1/2)
    if (character2) {
        switch (event->key()) {
        case Qt::Key_Left:  character2->setLeftDown(true); break;
        case Qt::Key_Right: character2->setRightDown(true); break;
        case Qt::Key_Up:    character2->setJumpDown(true); break;
        case Qt::Key_Down:  character2->setCrouchDown(true); break;
        case Qt::Key_1:     character2->setFireDown(true); break; // Numpad 1
        default: break;
        }
    }
    Scene::keyPressEvent(event);
}

void BattleScene::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;

    // Player 1 Controls
    if (character1) {
        switch (event->key()) {
        case Qt::Key_A: character1->setLeftDown(false); break;
        case Qt::Key_D: character1->setRightDown(false); break;
        case Qt::Key_W: character1->setJumpDown(false); break;
        case Qt::Key_S: character1->setCrouchDown(false); break;
        case Qt::Key_J: character1->setFireDown(false); break;
        default: break;
        }
    }

    // Player 2 Controls
    if (character2) {
        switch (event->key()) {
        case Qt::Key_Left:  character2->setLeftDown(false); break;
        case Qt::Key_Right: character2->setRightDown(false); break;
        case Qt::Key_Up:    character2->setJumpDown(false); break;
        case Qt::Key_Down:  character2->setCrouchDown(false); break;
        case Qt::Key_1:     character2->setFireDown(false); break;
        default: break;
        }
    }
    Scene::keyReleaseEvent(event);
}

// [MODIFIED] 主循环现在顺序更清晰
void BattleScene::update() {
    // 基类 update 会计算 deltaTime
    Scene::update();
}

// [MODIFIED] 拾取逻辑
void BattleScene::processPicking() {
    QList<Character*> characters = {character1, character2};
    for (Character* character : characters) {
        if (character && character->isPicking()) {
            auto mountable = findNearestUnmountedMountable(character->pos());
            if (mountable) {
                pickupMountable(character, mountable);
            }
        }
    }
}

Mountable *BattleScene::findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold) {
    Mountable *nearest = nullptr;
    qreal minDistance = distance_threshold;

    // 遍历场景中的所有可拾取物品
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

Mountable* BattleScene::pickupMountable(Character *character, Mountable *mountable) {
    // 使用 dynamic_cast 检查物品类型并调用对应的拾取函数
    if (auto armor = dynamic_cast<Armor *>(mountable)) {
        return character->pickupArmor(armor);
    }
    if (auto weapon = dynamic_cast<Weapon *>(mountable)) {
        return character->pickupWeapon(weapon);
    }
    return nullptr;
}
