#include "Knife.h"
#include "../Characters/Character.h"

Knife::Knife(QGraphicsItem *parent)
    : Weapon(parent, ":/Items/Weapons/Knife/knife.png", 25) { // 小刀有图片，攻击力为25
}

void Knife::attack(Character *attacker) {
    // 小刀攻击的特殊效果
    // 可以在这里添加小刀攻击的动画或音效
    Weapon::attack(attacker);
}