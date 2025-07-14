#include "SniperRifle.h"
#include "../Characters/Character.h"
#include "Bullet.h"
#include "Fist.h"
#include <QGraphicsScene>
#include <QDebug>

SniperRifle::SniperRifle(QGraphicsItem *parent)
    // 伤害更高 (50)，弹药更少 (5)，子弹速度更快
    : Weapon(parent, "50"), ammo(5)
{
    if (!pixmapItem) {
        pixmapItem = new QGraphicsPixmapItem(this);
    }
    // 设置拳头的图片
    if (pixmapItem) {
        pixmapItem->setPixmap(QPixmap(":/SniperRifle.png"));
        // 设置拳头相对于角色的位置（右手位置）
        pixmapItem->setPos(30, -10); // 根据角色图片调整具体数值
    }
    timer.start();
    lastShotTime = -COOLDOWN;
}

void SniperRifle::attack(Character *attacker) {
    if (ammo <= 0) return;
    if (timer.elapsed() - lastShotTime < COOLDOWN) return;

    lastShotTime = timer.elapsed();
    ammo--;
    qDebug() << "Sniper Rifle fired! Ammo left:" << ammo;

    if (attacker && attacker->scene()) {
        // 狙击枪子弹速度更快
        Bullet* bullet = new Bullet(attacker, this,this->getAttackPower(), 40.0);
        bullet->setPos(attacker->scenePos() + attacker->boundingRect().center()+QPointF(0, -10));
        attacker->scene()->addItem(bullet);
    }

    if (ammo <= 0) {
        qDebug() << "Sniper Rifle ran out of ammo. Switching to Fist.";
        attacker->setWeapon(new Fist(attacker));
        if (this->scene()) {
            this->scene()->removeItem(this);
        }
        delete this;
    }
}
