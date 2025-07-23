#ifndef SNIPERRIFLE_H
#define SNIPERRIFLE_H

#include "Weapon.h"
#include <QElapsedTimer>

class SniperRifle : public Weapon {
public:
    explicit SniperRifle(QGraphicsItem *parent = nullptr);
    void attack(Character *attacker) override;
    [[nodiscard]] WeaponType getWeaponType() const override { return WeaponType::SniperRifle; }
    // 【新增】检查武器是否因用完而被标记删除 (仿照 ShotPut)
    [[nodiscard]] bool isMarkedForDeletion() const;

private:
    // 【新增】内部使用的标记方法 (仿照 ShotPut)
    void markForDeletion();

    int ammo;
    long lastShotTime;
    static const int COOLDOWN = 1500; // 狙击枪射速慢
    QElapsedTimer timer;
    // 【新增】删除标记 (仿照 ShotPut)
    bool m_markedForDeletion;
};

#endif // SNIPERRIFLE_H
