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
    
    // 新增：获取攻击范围的getter函数
    [[nodiscard]] qreal getAttackRange() const;

    void attack(Character *attacker);

protected:
    int attackPower;
    // 新增：攻击范围属性
    qreal attackRange{100.0};  // 默认攻击范围为100像素
};

#endif