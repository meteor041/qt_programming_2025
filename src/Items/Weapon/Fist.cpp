#include "Fist.h"
#include "../Characters/Character.h"

Fist::Fist(QGraphicsItem *parent)
    : Weapon(parent, "", 10) { // 拳头没有图片，攻击力为10
}

void Fist::attack(Character *attacker) {
    // 拳头攻击的特殊效果
    // 可以在这里添加拳头攻击的动画或音效
    Weapon::attack(attacker);
}