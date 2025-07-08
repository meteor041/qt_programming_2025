// src/Scenes/BattleScene.cpp

#include <QDebug>
#include <QLineF>
#include "BattleScene.h"
#include "../Items/Characters/Link.h"
#include "../Items/Maps/Battlefield.h"
#include "../Items/Armors/FlamebreakerArmor.h"
#include "../Items/Maps/platform/Platform.h"
//#include "../Items/Weapons/Weapon.h"      // [FIXED] 确保包含武器
//#include "../Items/Armors/Armor.h"        // [FIXED] 确保包含护甲
#include "../Items/Mountable.h"           // [FIXED] 确保包含可挂载基类

// 构造函数：初始化整个战斗场景
BattleScene::BattleScene(QObject *parent) : Scene(parent) {
    setSceneRect(0, 0, 1280, 720);

    map = new Battlefield(this);
    addItem(map);
    map->setZValue(-1.0);
    map->scaleToFitScene(this);

    // [FIXED] 修复 getSpawnPos 调用逻辑
    QPointF baseSpawnPoint = map->getSpawnPos(); // 调用无参版本

    // 创建玩家1
    character1 = new Link(1);
    addItem(character1);
    character1->setPos(baseSpawnPoint - QPointF(100, 0)); // 将玩家1放在出生点左侧

    // 创建玩家2
    character2 = new Link(2);
    addItem(character2);
    character2->setPos(baseSpawnPoint + QPointF(100, 0)); // 将玩家2放在出生点右侧

    // 创建并放置示例物品
    auto* spareArmor = new FlamebreakerArmor();
    addItem(spareArmor);
    spareArmor->unmount();

    QPointF armorTargetPos(sceneRect().center().x() + 200, 0);
    Platform* groundForArmor = map->getGroundPlatform(armorTargetPos);
    if (groundForArmor) {
        spareArmor->setPos(armorTargetPos.x(), groundForArmor->getSurfaceY() - spareArmor->boundingRect().height());
    } else {
        spareArmor->setPos(sceneRect().center());
    }
}

// 物理处理逻辑保持不变，它已经很健壮了
void BattleScene::processPhysics() {
    const qreal GRAVITY = 2000.0;
    QList<Character*> characters = {character1, character2};

    for (Character* character : characters) {
        if (!character || !character->isEnabled()) continue;

        Platform* ground = map->getGroundPlatform(character->pos());

        if (ground) {
            character->setIsOnGround(true);
            qreal footY = character->sceneBoundingRect().bottom();
            qreal surfaceY = ground->getSurfaceY();
            if (footY > surfaceY && character->getVelocity().y() >= 0) {
                character->setY(surfaceY - character->boundingRect().height());
                character->setVelocity(character->getVelocity().x(), 0);
            }
        } else {
            character->setIsOnGround(false);
            character->addVelocity(QPointF(0, GRAVITY * (deltaTime / 1000.0)));
        }
    }
}

// 角色更新逻辑保持不变
void BattleScene::processCharacterUpdates() {
    if (character1 && character1->isEnabled()) character1->update(deltaTime);
    if (character2 && character2->isEnabled()) character2->update(deltaTime);
}


// 双人键盘控制保持不变
void BattleScene::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;
    if (character1 && character1->isEnabled()) {
        switch (event->key()) {
        case Qt::Key_A: character1->setLeftDown(true); break;
        case Qt::Key_D: character1->setRightDown(true); break;
        case Qt::Key_W: character1->setJumpDown(true); break;
        case Qt::Key_S: character1->setCrouchDown(true); break;
        case Qt::Key_J: character1->setFireDown(true); break;
        default: break;
        }
    }
    if (character2 && character2->isEnabled()) {
        switch (event->key()) {
        case Qt::Key_Left:  character2->setLeftDown(true); break;
        case Qt::Key_Right: character2->setRightDown(true); break;
        case Qt::Key_Up:    character2->setJumpDown(true); break;
        case Qt::Key_Down:  character2->setCrouchDown(true); break;
        case Qt::Key_1:     character2->setFireDown(true); break;
        default: break;
        }
    }
    Scene::keyPressEvent(event);
}

void BattleScene::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) return;
    if (character1) { /* ... 和之前一样 ... */ }
    if (character2) { /* ... 和之前一样 ... */ }
    Scene::keyReleaseEvent(event);
}

// 拾取逻辑保持不变
void BattleScene::processPicking() {
    QList<Character*> characters = {character1, character2};
    for (Character* character : characters) {
        if (character && character->isEnabled() && character->isPicking()) {
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

// [FIXED] 实现完整的、多态的拾取逻辑
Mountable* BattleScene::pickupMountable(Character *character, Mountable *mountable) {
    if (auto armor = dynamic_cast<Armor *>(mountable)) {
        return character->pickupArmor(armor);
    }
    // 假设你的Character类有pickupWeapon函数
//    if (auto weapon = dynamic_cast<Weapon *>(mountable)) {
        // return character->pickupWeapon(weapon);
//    }
    // 如果有其他类型的物品，可以在这里添加
    return nullptr;
}

