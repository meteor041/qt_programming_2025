// --- START OF FILE bullet.h (修改后) ---
#ifndef BULLET_H
#define BULLET_H

#include "../Item.h"
#include "../Weapon/Weapon.h" // 【重要】包含完整的Weapon头文件以获取WeaponType枚举

class Character; // 前向声明

class Bullet : public Item {
public:
    // 【修改】构造函数不再接收 Weapon*，而是接收 WeaponType
    explicit Bullet(Character* owner, int damage, WeaponType sourceType, qreal speed, QGraphicsItem* parent = nullptr);

    // 每一帧更新子弹位置
    void advance(int phase) override;

    // 获取子弹的伤害值
    [[nodiscard]] int getDamage() const;

    // 获取发射者
    [[nodiscard]] Character* getOwner() const;

    // 添加一个公共方法来检查是否被标记
    [[nodiscard]] bool isMarkedForDeletion() const;

protected:
    // 碰撞检测
    void checkCollision();

private:
    Character* owner;         // 发射这颗子弹的角色，防止伤害到自己
    int damage;               // 子弹伤害
    WeaponType m_sourceType;  // 【修改】存储武器类型，而不是指针
    qreal speed;              // 子弹飞行速度
    QPointF velocity;         // 子弹的速度向量
    bool markedForDeletion;   // 删除标记
};

#endif // BULLET_H
