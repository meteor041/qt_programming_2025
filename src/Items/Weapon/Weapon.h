#ifndef QT_PROGRAMMING_2024_WEAPON_H
#define QT_PROGRAMMING_2024_WEAPON_H

#include "../Item.h"
#include "../Mountable.h"

// 前向声明，避免循环依赖
class Character;

class Weapon : public Item, public Mountable {
public:
    explicit Weapon(QGraphicsItem *parent, const QString &attackPower);

    [[nodiscard]] int getAttackPower() const;

    void attack(Character *attacker);

protected:
    int attackPower;
};

#endif