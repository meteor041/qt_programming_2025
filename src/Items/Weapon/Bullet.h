#ifndef BULLET_H
#define BULLET_H

#include "../Item.h"

class Character; // 前向声明

class Bullet : public Item {
public:
    explicit Bullet(Character* owner, int damage, qreal speed, QGraphicsItem* parent = nullptr);

    // 每一帧更新子弹位置
    void advance(int phase) override;

    // 获取子弹的伤害值
    [[nodiscard]] int getDamage() const;

    // 获取发射者
    [[nodiscard]] Character* getOwner() const;

    // 【新增】添加一个公共方法来检查是否被标记
    [[nodiscard]] bool isMarkedForDeletion() const;

protected:
    // 碰撞检测
    void checkCollision();

private:
    Character* owner;      // 发射这颗子弹的角色，防止伤害到自己
    int damage;            // 子弹伤害
    qreal speed;           // 子弹飞行速度
    QPointF velocity;      // 子弹的速度向量
    bool markedForDeletion; // 【新增】添加一个布尔标记
};

#endif // BULLET_H
