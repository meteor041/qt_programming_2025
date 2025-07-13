#ifndef RIFLE_H
#define RIFLE_H

#include "Weapon.h"
#include <QElapsedTimer> // 用于计算射击间隔

class Rifle : public Weapon {
public:
    explicit Rifle(QGraphicsItem *parent = nullptr);

    // 重写攻击方法
    void attack(Character *attacker) override;

    // 重写武器类型获取
    [[nodiscard]] WeaponType getWeaponType() const override { return WeaponType::Rifle; }

private:
    int ammo;                // 弹药数量
    long lastShotTime;       // 上次射击的时间戳
    static const int COOLDOWN = 200; // 射击间隔，单位：毫秒
    QElapsedTimer timer;     // 计时器
};

#endif // RIFLE_H
