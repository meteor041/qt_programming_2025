// src/Items/Weapon/ShotPutProjectile.h

#ifndef SHOTPUTPROJECTILE_H
#define SHOTPUTPROJECTILE_H

#include "../Item.h"

// 前向声明，避免循环依赖
class Character;
class Weapon;

class ShotPutProjectile : public Item {
public:
    explicit ShotPutProjectile(Character* owner, Weapon* sourceWeapon, int damage, const QPointF& initialVelocity, QGraphicsItem* parent = nullptr);

    // 每一帧更新位置，模拟抛物线
    void advance(int phase) override;

    // 检查是否被标记为待删除
    [[nodiscard]] bool isMarkedForDeletion() const;

protected:
    void checkCollision();

private:
    Character* owner;           // 投掷者
    Weapon* sourceWeapon;       // 伤害来源武器
    int damage;                 // 伤害值
    QPointF velocity;           // 速度向量 (会受重力影响)
    bool markedForDeletion;     // 待删除标记
    static constexpr qreal GRAVITY = 0.8; // 重力加速度
};

#endif // SHOTPUTPROJECTILE_H
