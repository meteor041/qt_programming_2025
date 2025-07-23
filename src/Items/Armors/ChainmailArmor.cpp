// src/Items/Armors/ChainmailArmor.cpp

#include "ChainmailArmor.h"
#include <QDebug>

ChainmailArmor::ChainmailArmor(QGraphicsItem *parent)
    : Armor(parent, ":/ChainmailArmor.png")
{
    // 锁子甲没有耐久度概念，但我们可以将其设为-1或一个大数以示区分
    m_durability = -1; // -1 表示无限耐久
    m_maxDurability = -1;
}

int ChainmailArmor::processDamage(int incomingDamage, WeaponType sourceType) {


    WeaponType type = sourceType;

    switch (type) {
    case WeaponType::Fist:
        qDebug() << "Chainmail Armor: Immune to Fist damage.";
        return 0; // 免疫拳头伤害

    case WeaponType::Knife:
        qDebug() << "Chainmail Armor: Reducing Knife damage.";
        // 假设减免50%的小刀伤害
        return incomingDamage / 2;

    default:
        // 对其他武器无效
        return incomingDamage;
    }
}

QString ChainmailArmor::getName() const {
    return "Chainmail";
}
