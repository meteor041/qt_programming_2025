#ifndef SNIPERRIFLE_H
#define SNIPERRIFLE_H

#include "Weapon.h"
#include <QElapsedTimer>

class SniperRifle : public Weapon {
public:
    explicit SniperRifle(QGraphicsItem *parent = nullptr);
    void attack(Character *attacker) override;
    [[nodiscard]] WeaponType getWeaponType() const override { return WeaponType::SniperRifle; }

private:
    int ammo;
    long lastShotTime;
    static const int COOLDOWN = 1500; // 狙击枪射速慢
    QElapsedTimer timer;
};

#endif // SNIPERRIFLE_H
