#ifndef QT_PROGRAMMING_2024_SHOTPUT_H
#define QT_PROGRAMMING_2024_SHOTPUT_H

#include "Weapon.h"
#include <QGraphicsEllipseItem>
#include <QTimer>
#include <QList>

// 前向声明
class ShotPutProjectile;
class BattleScene;

class ShotPut : public QObject, public Weapon  {
    Q_OBJECT

public:
    explicit ShotPut(QGraphicsItem *parent = nullptr, int throwCount = 3);

    // 重写攻击方法，实现投掷功能
    void attack(Character *attacker) override;

    // 【新增】实现基类的纯虚函数
    [[nodiscard]] WeaponType getWeaponType() const override;

    // 获取剩余投掷次数
    [[nodiscard]] int getRemainingThrows() const;

    // 检查是否还能使用
    [[nodiscard]] bool canThrow() const;

    static constexpr int DAMAGE = 25;

    // 新增：析构函数
    ~ShotPut() override;

    // 新增：注册投掷物
    void registerProjectile(ShotPutProjectile* projectile);

    // 新增：注销投掷物
    void unregisterProjectile(ShotPutProjectile* projectile);

private:
    int maxThrows;        // 最大投掷次数
    int remainingThrows;  // 剩余投掷次数

    // 投掷实心球
    void throwShotPut(Character *attacker);

    // 新增：追踪活跃的投掷物
    QList<ShotPutProjectile*> activeProjectiles;
};

// 投掷物类 - 处理斜抛运动
class ShotPutProjectile : public QGraphicsEllipseItem {

public:
    explicit ShotPutProjectile(QPointF startPos, bool facingRight,
                               Character *thrower, BattleScene *scene,
                               ShotPut *weapon,
                               QGraphicsItem *parent = nullptr);

    // 添加析构函数
    ~ShotPutProjectile();

    // 更新投掷物位置
    void updatePosition();

    // 检查碰撞
    void checkCollision();

    // 新增：检查是否标记为删除
    [[nodiscard]] bool isMarkedForDeletion() const;

    // 新增：设置投掷者为空（当投掷者被销毁时调用）
    void clearThrower() { thrower = nullptr; }

    // 新增：设置源武器为空（当武器被销毁时调用）
    void clearSourceWeapon() { sourceWeapon = nullptr; }

private:
    QPointF velocity;        // 当前速度
    QPointF initialVelockity; // 初始速度
    Character *thrower;      // 投掷者
    BattleScene *battleScene; // 战斗场景引用
    ShotPut *sourceWeapon;  // 新增：保存武器引用
    bool markedForDeletion;  // 添加成员变量声明

    static constexpr qreal GRAVITY = 0.8;     // 重力加速度
    static constexpr qreal INITIAL_SPEED = 15.0; // 初始速度
    static constexpr qreal THROW_ANGLE = 30.0;   // 投掷角度(度)
};

#endif // QT_PROGRAMMING_2024_SHOTPUT_H
