#include "Fist.h"
#include "../Characters/Character.h"
#include <QDebug>
#include <QPixmap>

// 构造函数：拳头的攻击力设为5
Fist::Fist(QGraphicsItem *parent) 
    : Weapon(parent, "5") {
    // 手动创建 pixmapItem，因为 Weapon 基类没有创建
    if (!pixmapItem) {
        pixmapItem = new QGraphicsPixmapItem(this);
    }
    // 设置拳头的图片
    if (pixmapItem) {
        pixmapItem->setPixmap(QPixmap(":/fist.png"));
        // 设置拳头相对于角色的位置（右手位置）
        pixmapItem->setPos(30, -10); // 根据角色图片调整具体数值
    }
    // 拳头是默认武器，创建时自动装备
    mountToParent();
}

// 重写攻击方法，实现拳头特有的攻击效果
void Fist::attack(Character *attacker) {
    if (attacker == nullptr) {
        qDebug() << "Warning: Attacker is null, cannot perform fist attack";
        return;
    }
    
    // 拳头不需要检查是否装备，因为它是默认武器
    qDebug() << "Character throws a punch! Damage:" << getAttackPower();
    
    // 拳头攻击的特殊效果
    // - 攻击速度快
    // - 攻击距离短
    // - 可以连击
    
    // TODO: 实现拳头特有的攻击逻辑
    // - 近距离攻击检测
    // - 快速连击系统
    // - 拳击音效
}