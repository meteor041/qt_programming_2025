// src/Items/Armors/ChainmailArmor.h

#ifndef QT_PROGRAMMING_2024_CHAINMAILARMOR_H
#define QT_PROGRAMMING_2024_CHAINMAILARMOR_H

#include "Armor.h"

class ChainmailArmor : public Armor {
public:
    ChainmailArmor(QGraphicsItem* parent = nullptr);

    [[nodiscard]] QString getName() const override;
    // 实现伤害处理逻辑
    int processDamage(int incomingDamage, Weapon* sourceWeapon) override;
};

#endif //QT_PROGRAMMING_2024_CHAINMAILARMOR_H
