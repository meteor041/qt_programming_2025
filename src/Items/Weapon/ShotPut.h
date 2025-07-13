// src/Items/Weapon/ShotPut.h

#ifndef SHOTPUT_H
#define SHOTPUT_H

#include "Weapon.h"

class ShotPut : public Weapon {
public:
    explicit ShotPut(QGraphicsItem *parent = nullptr, int uses = 3);

    // 重写攻击方法，用于投掷实心球
    void attack(Character *attacker) override;

    // 获取武器类型
    [[nodiscard]] WeaponType getWeaponType() const override { return WeaponType::ShotPut; }

    // 检查武器是否因用完而被标记删除
    [[nodiscard]] bool isMarkedForDeletion() const;

private:
    void markForDeletion(); // 内部使用的标记方法

    int usesLeft;             // 剩余使用次数
    bool markedForDeletion;   // 是否被标记为待删除

    // 武器和投掷物参数
    static constexpr int ATTACK_POWER = 35;
    static constexpr qreal INITIAL_VELOCITY_X = 15.0; // 水平初速度
    static constexpr qreal INITIAL_VELOCITY_Y = -8.0; // 垂直初速度 (负值为向上)
};

#endif // SHOTPUT_H
