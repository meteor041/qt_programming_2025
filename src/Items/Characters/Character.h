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
#include "../Item.h" // 确保包含了Item.h

// 前向声明 Platform 类，避免循环引用
class Platform;

//人物状态，用于动画显示
enum CharacterState {
    Standing,
    Running,
    Jumping,
    Crouching,
    Attacking,
    Hit
};

class Character : public Item {
public:
    explicit Character(QGraphicsItem *parent);

    //由于character类需要保存很多图片，item类不应该修改，因此重写boundingRect
     [[nodiscard]] QRectF boundingRect() const override;

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

    // --- 【核心改动 1】: 添加被平台调用的公共方法 ---
    void setSpeedMultiplier(qreal multiplier);
    void setInStealth(bool stealth);

    // --- 【核心改动 2】: 用于在 BattleScene 中追踪平台状态 ---
    Platform* getCurrentPlatform() const;
    void setCurrentPlatform(Platform* platform);

    Armor* pickupArmor(Armor* newArmor);
    Armor* getArmor() const; // 【新增】获取当前护甲
    
    // 武器相关方法
    Weapon* getWeapon() const;
    void setWeapon(Weapon* newWeapon);
    void performAttack();
    
    // 新增：获取所持武器攻击范围的函数
    [[nodiscard]] qreal getWeaponAttackRange() const;
    [[nodiscard]] qreal getWeaponAttackPower() const;
    // 新增：生命值系统
    [[nodiscard]] int getHealth() const;
    [[nodiscard]] int getMaxHealth() const;
    [[nodiscard]] bool isDead() const;
    void setHealth(int health);
    void takeDamage(int damage,Weapon* sourceWeapon);
    void heal(int amount);
    
    // 新增：肾上腺素效果系统
    void startAdrenalineEffect(int duration, qreal speedMultiplier, int healPerFrame);
    void updateAdrenalineEffect(); // 在每帧更新中调用
    
    // 新增：朝向相关方法
    [[nodiscard]] bool isFacingRight() const;
    void setFacingRight(bool facingRight);

protected:
    HeadEquipment *headEquipment{};
    LegEquipment *legEquipment{};
    Armor *armor{};
    Weapon *weapon{};  // 当前装备的武器
    QPointF velocity{};
   QGraphicsEllipseItem *ellipseItem; // for debugging
private:
   // 【核心修改 A2】新增一个专门由Character管理的 QGraphicsPixmapItem
   QGraphicsPixmapItem *characterPixmapItem{};

    // 【新增】一个更新角色状态和外观的私有函数（实现下蹲功能时添加）
    void updateAppearanceAndState();

    bool leftDown{}, rightDown{}, pickDown{}, jumpDown{}, onGround{};
    bool lastPickDown{};
    bool picking{};
    //下蹲_按键是否按下
    bool crouchDown{};
    //角色是否处于下蹲的最终状态
    bool crouching{};
    
    // 【动画和状态管理变量】
    CharacterState currentState{Standing};
    CharacterState previousState{Standing}; // 新增：用于在一次性动画后恢复状态
    int animationFrameIndex{0};
    int animationFrameTimer{0};
    static const int ANIMATION_FRAME_DURATION = 8; // 动画帧切换速度
    // 【新增】受击状态计时器
    int hitStateTimer{0}; // > 0 表示处于受击状态，值为剩余帧数

    // 【核心修改】为所有状态准备图片资源
    QPixmap standingPixmap;                 // 站立时的图片
    QPixmap crouchingPixmap;                // 下蹲时的图片
    QList<QPixmap> runningAnimationFrames;  // 跑步动画序列
    QList<QPixmap> jumpingAnimationFrames;  // 跳跃动画序列
    QList<QPixmap> attackingAnimationFrames; // 新增：攻击动画序列
    QList<QPixmap> hitAnimationFrames;       // 新增：受击动画序列
    qreal standingHeight;
    qreal crouchingHeight;

    // --- 【核心改动 3】: 添加存储平台效果的私有成员变量 ---
    qreal m_speedMultiplier{1.0}; // 速度倍率，默认为1倍
    bool m_isInStealth{false};      // 是否处于隐身状态，默认为否
    Platform* m_currentPlatform{nullptr}; // 指向当前所在平台的指针

    // 新增：生命值相关变量
    int health{100};           // 当前生命值
    int maxHealth{100};        // 最大生命值
    bool dead{false};          // 是否死亡
    
    // 新增：肾上腺素效果相关变量
    bool adrenalineActive{false};     // 肾上腺素是否激活
    int adrenalineTimer{0};           // 肾上腺素剩余时间
    qreal adrenalineSpeedMultiplier{1.0}; // 肾上腺素速度倍率
    int adrenalineHealPerFrame{0};    // 每帧回血量
    int adrenalineHealCounter{0};     // 回血计数器
    
    // 新增：朝向相关变量
    bool facingRight{true};           // 角色朝向，默认朝右
};


#endif //QT_PROGRAMMING_2024_CHARACTER_H
