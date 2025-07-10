#include "Knife.h"
#include "../Characters/Character.h"
#include <QDebug>
#include <QPixmap>

// 构造函数：刀的攻击力设为15
Knife::Knife(QGraphicsItem *parent) 
    : Weapon(parent, "15") {
    // 设置刀的图片
    if (!pixmapItem) {
        pixmapItem = new QGraphicsPixmapItem(this);
    }
    if (pixmapItem) {
        pixmapItem->setPixmap(QPixmap(":/knife.png"));
        pixmapItem->setPos(30, -10);
    }
    // 刀需要手动装备
}

// 重写攻击方法，实现刀具特有的攻击效果
void Knife::attack(Character *attacker) {
    if (attacker == nullptr) {
        qDebug() << "Warning: Attacker is null, cannot perform knife attack";
        return;
    }
    
    // 检查刀是否已装备
    if (!isMounted()) {
        qDebug() << "Warning: Knife is not equipped, cannot attack";
        return;
    }
    
    qDebug() << "Character attacks with knife! Damage:" << getAttackPower();
    
    // 默认使用挥砍攻击
    slash(attacker);
}

// 挥砍攻击
void Knife::slash(Character *attacker) {
    if (attacker == nullptr) {
        return;
    }
    
    qDebug() << "Performing slash attack with knife!";
    
    // 挥砍攻击的特点：
    // - 中等攻击距离
    // - 横向攻击范围较大
    // - 攻击速度中等
    
    // TODO: 实现挥砍攻击逻辑
    // - 扇形攻击范围检测
    // - 挥砍动画
    // - 金属碰撞音效
}

// 刺击攻击
void Knife::stab(Character *attacker) {
    if (attacker == nullptr) {
        return;
    }
    
    qDebug() << "Performing stab attack with knife!";
    
    // 刺击攻击的特点：
    // - 攻击距离较远
    // - 直线攻击，范围较小
    // - 攻击速度快，伤害高
    
    // TODO: 实现刺击攻击逻辑
    // - 直线攻击范围检测
    // - 刺击动画
    // - 穿刺音效
}