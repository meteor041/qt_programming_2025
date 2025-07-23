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

    // 【新增】检查武器是否因用完而被标记删除 (仿照 ShotPut)
    [[nodiscard]] bool isMarkedForDeletion() const;
private:
    // 【新增】内部使用的标记方法 (仿照 ShotPut)
    void markForDeletion();
    int ammo;                // 弹药数量
    long lastShotTime;       // 上次射击的时间戳
    static const int COOLDOWN = 200; // 射击间隔，单位：毫秒
    QElapsedTimer timer;     // 计时器
    // 【新增】删除标记 (仿照 ShotPut)
    bool m_markedForDeletion;
};

#endif // RIFLE_H
