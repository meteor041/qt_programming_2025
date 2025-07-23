// src/Items/Armors/BulletproofVest.cpp

#include "BulletproofVest.h"
#include <QDebug>

BulletproofVest::BulletproofVest(QGraphicsItem *parent)
    : Armor(parent, ":/BulletproofVest.png") // 请替换为你的资源路径
{
    // 防弹衣有自己的耐久度
    m_durability = 150;
    m_maxDurability = 150;
}

int BulletproofVest::processDamage(int incomingDamage, WeaponType sourceType) {


    WeaponType type = sourceType;

    // 假设防弹衣只对 Rifle 和 SniperRifle 有效
    if (type == WeaponType::Rifle || type == WeaponType::SniperRifle) {
        // 假设防弹衣承担80%的伤害
        int damageToVest = incomingDamage * 0.8;
        int damageToPlayer = incomingDamage - damageToVest;

        m_durability -= damageToVest;
        qDebug() << "Bulletproof Vest absorbs" << damageToVest << "damage. Durability left:" << m_durability;

        if (m_durability <= 0) {
            m_isBroken = true;
            qDebug() << "Bulletproof Vest broke!";
            // 如果耐久度耗尽，剩余的伤害也应由玩家承受
            damageToPlayer += -m_durability;
            m_durability = 0;
            // 不再直接删除，而是调用基类的标记方法
            this->markForDeletion();
            // --- 修改结束 ---
        }

        return damageToPlayer;
    }

    // 对其他类型武器无效
    return incomingDamage;
}

QString BulletproofVest::getName() const {
    return "Bulletproof Vest";
}
