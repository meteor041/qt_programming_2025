// src/Items/Weapon/ShotPutProjectile.cpp

#include "ShotPutProjectile.h"
#include "../Characters/Character.h"
#include "../Weapon/Weapon.h"
#include <QGraphicsScene>
#include <QList>
#include <QDebug>

ShotPutProjectile::ShotPutProjectile(Character* owner, Weapon* sourceWeapon, int damage, const QPointF& initialVelocity, QGraphicsItem* parent)
    : Item(parent, ":/shotput.png"), // 假设你有一个名为 shotput.png 的资源
    owner(owner),
    sourceWeapon(sourceWeapon),
    damage(damage),
    velocity(initialVelocity),
    markedForDeletion(false)
{
    // 可以根据需要设置图片大小
    setScale(1);
}

void ShotPutProjectile::advance(int phase) {
    if (phase == 0 || markedForDeletion) {
        return;
    }

    // 模拟重力：垂直速度持续增加
    velocity.setY(velocity.y() + GRAVITY);

    // 更新位置
    setPos(pos() + velocity);

    // 碰撞检测
    checkCollision();

    // 飞出屏幕边界时，标记删除
    if (!scene()->sceneRect().contains(pos())) {
        qDebug() << "ShotPutProjectile out of bounds. Marking for deletion.";
        markedForDeletion = true;
    }
}

void ShotPutProjectile::checkCollision() {
    if (markedForDeletion) {
        return;
    }

    QList<QGraphicsItem*> colliding_items = collidingItems();
    for (QGraphicsItem* item : colliding_items) {
        if (auto* target = dynamic_cast<Character*>(item)) {
            // 如果击中了非自己且存活的角色
            if (target != owner && !target->isDead()) {
                qDebug() << "ShotPutProjectile hit a character! Marking for deletion.";
                target->takeDamage(this->damage, this->sourceWeapon);
                markedForDeletion = true;
                return; // 命中后立即返回
            }
        }
    }
}

bool ShotPutProjectile::isMarkedForDeletion() const {
    return markedForDeletion;
}
