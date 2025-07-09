//
// Created by AI Assistant
//

#include "Weapon.h"
#include "../Characters/Character.h"
#include <QDebug>

// 构造函数实现
Weapon::Weapon(QGraphicsItem *parent, const QString &attackPower) 
    : Item(parent, ""), Mountable() {
    // 将攻击力字符串转换为整数
    bool ok;
    this->attackPower = attackPower.toInt(&ok);
    if (!ok) {
        // 如果转换失败，设置默认攻击力
        this->attackPower = 10;
        qDebug() << "Warning: Invalid attack power string, using default value 10";
    }
}

// 在现有代码基础上添加以下方法实现

// 获取攻击力
int Weapon::getAttackPower() const {
    return attackPower;
}

// 新增：获取攻击范围
qreal Weapon::getAttackRange() const {
    return attackRange;
}

// 攻击方法实现
void Weapon::attack(Character *attacker) {
    if (attacker == nullptr) {
        qDebug() << "Warning: Attacker is null, cannot perform attack";
        return;
    }
    
    // 检查武器是否已装备
    if (!isMounted()) {
        qDebug() << "Warning: Weapon is not mounted, cannot attack";
        return;
    }
    
    // 执行攻击逻辑
    qDebug() << "Character attacks with weapon! Attack power:" << attackPower;
    
    // 这里可以添加更多攻击逻辑，比如：
    // - 播放攻击动画
    // - 计算伤害
    // - 检测攻击范围内的敌人
    // - 应用伤害效果
    
    // 示例：简单的攻击效果
    // TODO: 实现具体的攻击逻辑，如伤害计算、范围检测等
}