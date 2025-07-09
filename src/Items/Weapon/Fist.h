#ifndef QT_PROGRAMMING_2024_FIST_H
#define QT_PROGRAMMING_2024_FIST_H

#include "Weapon.h"

class Fist : public Weapon {
public:
    explicit Fist(QGraphicsItem *parent = nullptr);
    
    // 重写攻击方法，实现拳头特有的攻击效果
    void attack(Character *attacker) ;
};

#endif // QT_PROGRAMMING_2024_FIST_H
