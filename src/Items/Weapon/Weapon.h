#ifndef QT_PROGRAMMING_2024_WEAPON_H
#define QT_PROGRAMMING_2024_WEAPON_H

#include "../Item.h"
#include "../Mountable.h"

class Weapon : public Item, public Mountable {
    public explicit Weapon(QGraphicsItem *parent, const QString &attackPower);

    [[nodiscard]] int getAttackPower() const;

    void attack(Character *attacker);

    protected:
        int attackPower;
}

#endif