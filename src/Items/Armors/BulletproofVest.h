// src/Items/Armors/BulletproofVest.h

#ifndef QT_PROGRAMMING_2024_BULLETPROOFVEST_H
#define QT_PROGRAMMING_2024_BULLETPROOFVEST_H

#include "Armor.h"

class BulletproofVest : public Armor {
public:
    BulletproofVest(QGraphicsItem* parent = nullptr);

    [[nodiscard]] QString getName() const override;
    // 实现伤害处理逻辑
    int processDamage(int incomingDamage, Weapon* sourceWeapon) override;
};

#endif //QT_PROGRAMMING_2024_BULLETPROOFVEST_H
