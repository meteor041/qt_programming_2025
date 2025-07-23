#include "Rifle.h"
#include "../Characters/Character.h"
#include "../../Scenes/BattleScene.h"
#include "Bullet.h"
#include "Fist.h" // 当子弹用完时，换回拳头
#include <QGraphicsScene>
#include <QDebug>

Rifle::Rifle(QGraphicsItem *parent)
    : Weapon(parent, "10"), ammo(30),m_markedForDeletion(false) // 【新增】初始化标记 // 假设攻击力10，弹药30
{
    // Weapon构造函数第一个参数是parent, 第二个是攻击力
    // 设置贴图
    if (!pixmapItem) {
        pixmapItem = new QGraphicsPixmapItem(this);
    }
    // 设置拳头的图片
    if (pixmapItem) {
        pixmapItem->setPixmap(QPixmap(":/Rifle.png"));
        // 设置拳头相对于角色的位置（右手位置）
        pixmapItem->setPos(30, -10); // 根据角色图片调整具体数值
    }
    // 启动计时器
    timer.start();
    lastShotTime = -COOLDOWN; // 确保第一次可以立即开火
}

void Rifle::attack(Character *attacker) {
    // 检查弹药和射击冷却
    if (ammo <= 0) {
        qDebug() << "Rifle is out of ammo!";
        return;
    }

    if (timer.elapsed() - lastShotTime < COOLDOWN) {
        // 还没冷却好
        return;
    }

    // 更新上次射击时间
    lastShotTime = timer.elapsed();

    // 消耗一发子弹
    ammo--;
    qDebug() << "Rifle fired! Ammo left:" << ammo;

    // --- 核心逻辑：创建子弹 ---
    if (attacker && attacker->scene()) {
        // 创建子弹实例
        // 参数: owner, damage, speed
        // 【核心修正】创建子弹时，把 this (指向当前这把 Rifle 的指针) 传进去
        Bullet* bullet = new Bullet(attacker, this->getAttackPower(), this->getWeaponType(), 25.0);

        // 设置子弹的初始位置 (例如，在角色的中心)
        bullet->setPos(attacker->scenePos() + attacker->boundingRect().center()+QPointF(0, -10));

        // 将子弹添加到场景中
        attacker->scene()->addItem(bullet);
    }

    // 如果子弹打完了
    if (ammo <= 0) {
        qDebug() << "Rifle ran out of ammo. Switching to Fist.";
        // 卸载自己，并让角色换上拳头
        attacker->setWeapon(new Fist(attacker)); // 角色自动装备拳头

        // 【核心修改】标记自己等待被 BattleScene 清理，而不是直接删除
        this->markForDeletion();
    }
}

// 【新增】实现标记和检查方法 (仿照 ShotPut)
bool Rifle::isMarkedForDeletion() const {
    return m_markedForDeletion;
}

void Rifle::markForDeletion() {
    m_markedForDeletion = true;
    // 隐藏武器图标，因为它即将被删除
    if (this->scene()) {
        // 这一步是可选的，但可以立即提供视觉反馈
        // BattleScene 的 processDeletions 最终会处理真正的移除和删除
    }
}
