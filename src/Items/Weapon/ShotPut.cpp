// src/Items/Weapon/ShotPut.cpp

#include "ShotPut.h"
#include "ShotPutProjectile.h"
#include "../Characters/Character.h"
#include "Fist.h"
#include <QGraphicsScene>
#include <QDebug>
#include <string>

ShotPut::ShotPut(QGraphicsItem *parent, int uses)
    : Weapon(parent, std::to_string(ATTACK_POWER).c_str()),
    usesLeft(uses),
    markedForDeletion(false)
{
    // 设置武器图标
    if (!pixmapItem) {
        pixmapItem = new QGraphicsPixmapItem(this);
    }
    pixmapItem->setPixmap(QPixmap(":/shotput.png")); // 假设有 shotput_icon.png 作为武器图标
    pixmapItem->setPos(30, -10);
}

void ShotPut::attack(Character *attacker) {
    if (usesLeft <= 0 || markedForDeletion || !attacker || !attacker->scene()) {
        return;
    }

    usesLeft--;
    qDebug() << "ShotPut used! Uses left:" << usesLeft;

    // 计算初始速度方向
    qreal direction = attacker->isFacingRight() ? 1.0 : -1.0;
    QPointF initialVelocity(INITIAL_VELOCITY_X * direction, INITIAL_VELOCITY_Y);

    // 创建投掷物实例
    auto* projectile = new ShotPutProjectile(attacker, this, this->getAttackPower(), initialVelocity);

    // 设置投掷物的初始位置
    projectile->setPos(attacker->scenePos() + attacker->boundingRect().center());

    // 将投掷物添加到场景中
    attacker->scene()->addItem(projectile);

    // 如果次数用完
    if (usesLeft <= 0) {
        qDebug() << "ShotPut ran out of uses. Switching to Fist and marking for deletion.";
        // 让角色换回拳头
        attacker->setWeapon(new Fist(attacker));
        // 标记自己等待被 BattleScene 清理
        this->markForDeletion();
    }
}

bool ShotPut::isMarkedForDeletion() const {
    return markedForDeletion;
}

void ShotPut::markForDeletion() {
    markedForDeletion = true;
    // 隐藏武器图标，因为它即将被删除
    if (this->scene()) {
        this->scene()->removeItem(this);
    }
}
