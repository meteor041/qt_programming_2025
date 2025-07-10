#ifndef QT_PROGRAMMING_2024_KNIFE_H
#define QT_PROGRAMMING_2024_KNIFE_H

#include "Weapon.h"

class Knife : public Weapon {
public:
    explicit Knife(QGraphicsItem *parent = nullptr);
    
    // 重写攻击方法，实现刀具特有的攻击效果
    void attack(Character *attacker);
    
    // 【新增】实现类型获取（狙击枪和步枪和实心球还未添加)
    [[nodiscard]] WeaponType getWeaponType() const override;

    // 刀具特有的方法
    void slash(Character *attacker);  // 挥砍攻击
    void stab(Character *attacker);   // 刺击攻击
};

#endif // QT_PROGRAMMING_2024_KNIFE_H
