//
// Created by gerw on 8/20/24.
//

#ifndef QT_PROGRAMMING_2024_CHARACTER_H
#define QT_PROGRAMMING_2024_CHARACTER_H

#include <QGraphicsEllipseItem>
#include "../HeadEquipments/HeadEquipment.h"
#include "../Armors/Armor.h"
#include "../LegEquipments/LegEquipment.h"
#include "../Weapon/Weapon.h"
#include <QPixmap>

class Character : public Item {
public:
    explicit Character(QGraphicsItem *parent);

    [[nodiscard]] bool isLeftDown() const;

    void setLeftDown(bool leftDown);

    [[nodiscard]] bool isRightDown() const;

    void setRightDown(bool rightDown);

    [[nodiscard]] bool isJumpDown() const;

    void setJumpDown(bool jumpDown);

    [[nodiscard]] bool isOnGround() const;

    void setOnGround(bool onGround);

    //新增的下蹲函数
    [[nodiscard]] bool isCrouchDown() const;

    void setCrouchDown(bool crouchDown);

    [[nodiscard]] bool isCrouching() const;
    //下蹲函数止

    [[nodiscard]] bool isPickDown() const;

    void setPickDown(bool pickDown);

    [[nodiscard]] const QPointF &getVelocity() const;

    [[nodiscard]] bool isPicking() const;

    void setVelocity(const QPointF &velocity);

    void processInput();

    // 【新增】一个更新角色状态和外观的私有函数（实现下蹲功能时添加）
    void updateAppearanceAndState();

    Armor* pickupArmor(Armor* newArmor);
    
    // 武器相关方法
    Weapon* getWeapon() const;
    void setWeapon(Weapon* newWeapon);
    void performAttack();
    
    // 新增：获取所持武器攻击范围的函数
    [[nodiscard]] qreal getWeaponAttackRange() const;
    
    // 新增：生命值系统
    [[nodiscard]] int getHealth() const;
    [[nodiscard]] int getMaxHealth() const;
    [[nodiscard]] bool isDead() const;
    void setHealth(int health);
    void takeDamage(int damage);
    void heal(int amount);

protected:
    HeadEquipment *headEquipment{};
    LegEquipment *legEquipment{};
    Armor *armor{};
    Weapon *weapon{};  // 当前装备的武器
    QPointF velocity{};
   QGraphicsEllipseItem *ellipseItem; // for debugging
private:
    bool leftDown{}, rightDown{}, pickDown{}, jumpDown{}, onGround{};
    bool lastPickDown{};
    bool picking{};
    //下蹲_按键是否按下
    bool crouchDown{};
    //角色是否处于下蹲的最终状态
    bool crouching{};
    
    // 跳跃计时器相关变量
    int jumpCooldownTimer{0};        // 跳跃冷却计时器
    static const int JUMP_COOLDOWN_FRAMES = 30; // 跳跃冷却帧数（约0.33秒，假设90FPS）
    QPixmap standingPixmap;//站立时的照片
    QPixmap crouchingPixmap;//下蹲时的照片
    qreal standingHeight;//站立照片高度
    qreal crouchingHeight;//下蹲照片高度



    // 新增：生命值相关变量
    int health{100};           // 当前生命值
    int maxHealth{100};        // 最大生命值
    bool dead{false};          // 是否死亡
};


#endif //QT_PROGRAMMING_2024_CHARACTER_H
