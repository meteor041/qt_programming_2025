#include "Bullet.h"
#include "../Characters/Character.h"
#include "../../Scenes/BattleScene.h"
#include "../Weapon/Weapon.h" // <-- 【新增】包含完整的
#include <QGraphicsScene>
#include <QList>
#include <QDebug>

Bullet::Bullet(Character* owner, Weapon* sourceWeapon, int damage, qreal speed, QGraphicsItem* parent)
    : Item(parent, ":/bullet1.png"), owner(owner), sourceWeapon(sourceWeapon),damage(damage), speed(speed), markedForDeletion(false) // <-- 初始化
{
    // 根据发射者的朝向决定子弹的初始速度方向
    if (owner->isFacingRight()) {
        velocity = QPointF(speed, 0);
    } else {
        velocity = QPointF(-speed, 0);
    }
}

// QGraphicsScene会自动在每帧调用advance()
void Bullet::advance(int phase) {
    if (phase == 0) return;

    // 如果已经被标记了，就直接返回，不再移动
    if (markedForDeletion) {
        return;
    }

    setPos(pos() + velocity);
    checkCollision();

    // 飞出屏幕边界时，也只是打上标记
    if (!scene()->sceneRect().contains(pos())) {
        qDebug() << "Bullet out of bounds. Marking for deletion.";
        markedForDeletion = true;
    }
}

void Bullet::checkCollision() {
    // 如果已经被标记，就没必要再检查碰撞了
    if (markedForDeletion) {
        return;
    }

    QList<QGraphicsItem*> colliding_items = collidingItems();

    for (QGraphicsItem* item : colliding_items) {
        if (auto* target = dynamic_cast<Character*>(item)) {
            if (target != owner && !target->isDead()) {
                qDebug() << "Bullet hit a character! Marking for deletion.";
                // 【核心修正】将存储的 sourceWeapon 指针传递过去！
                target->takeDamage(this->damage, this->sourceWeapon);
                // 【修改！】不再直接删除，而是打上标记
                markedForDeletion = true;

                // 立即返回，因为子弹已经完成使命，无需再检测与其他物体的碰撞
                return;
            }
        }
    }
}


// 【新增】实现 isMarkedForDeletion()
bool Bullet::isMarkedForDeletion() const {
    return markedForDeletion;
}

int Bullet::getDamage() const {
    return damage;
}

Character* Bullet::getOwner() const {
    return owner;
}
