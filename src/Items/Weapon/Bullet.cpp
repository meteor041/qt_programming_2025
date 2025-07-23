// --- START OF FILE Bullet.cpp (修改后) ---
#include "Bullet.h"
#include "../Characters/Character.h"
#include "../../Scenes/BattleScene.h"
// #include "../Weapon/Weapon.h" // 不再需要，因为已经在 .h 中包含了
#include <QGraphicsScene>
#include <QList>
#include <QDebug>

// 【修改】构造函数实现
Bullet::Bullet(Character* owner, int damage, WeaponType sourceType, qreal speed, QGraphicsItem* parent)
    : Item(parent, ":/bullet1.png"),
    owner(owner),
    damage(damage),
    m_sourceType(sourceType), // 【修改】初始化武器类型
    speed(speed),
    markedForDeletion(false)
{
    // 根据发射者的朝向决定子弹的初始速度方向
    if (owner->isFacingRight()) {
        velocity = QPointF(speed, 0);
    } else {
        velocity = QPointF(-speed, 0);
    }
}

void Bullet::advance(int phase) {
    if (phase == 0 || markedForDeletion) {
        return;
    }

    setPos(pos() + velocity);
    checkCollision();

    if (!scene()->sceneRect().contains(pos())) {
        qDebug() << "Bullet out of bounds. Marking for deletion.";
        markedForDeletion = true;
    }
}

void Bullet::checkCollision() {
    if (markedForDeletion) {
        return;
    }

    QList<QGraphicsItem*> colliding_items = collidingItems();

    for (QGraphicsItem* item : colliding_items) {
        if (auto* target = dynamic_cast<Character*>(item)) {
            if (target != owner && !target->isDead()) {
                qDebug() << "Bullet hit a character! Marking for deletion.";
                // 【核心修正】传递存储的武器类型，而不是指针！
                target->takeDamage(this->damage, this->m_sourceType);

                markedForDeletion = true;
                return;
            }
        }
    }
}

bool Bullet::isMarkedForDeletion() const {
    return markedForDeletion;
}

int Bullet::getDamage() const {
    return damage;
}

Character* Bullet::getOwner() const {
    return owner;
}
