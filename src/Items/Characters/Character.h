//
// Created by gerw on 8/20/24.
//

#ifndef QT_PROGRAMMING_2024_CHARACTER_H
#define QT_PROGRAMMING_2024_CHARACTER_H

#include <QGraphicsEllipseItem>
#include "../HeadEquipments/HeadEquipment.h"
#include "../Armors/Armor.h"
#include "../LegEquipments/LegEquipment.h"
#include <QPixmap>
#include "../Item.h" // 确保包含了Item.h

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

    Armor* pickupArmor(Armor* newArmor);



protected:
    HeadEquipment *headEquipment{};
    LegEquipment *legEquipment{};
    Armor *armor{};
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
    
    // 跳跃计时器相关变量
    int jumpCooldownTimer{0};        // 跳跃冷却计时器
    static const int JUMP_COOLDOWN_FRAMES = 30; // 跳跃冷却帧数（约0.33秒，假设90FPS）
    QPixmap standingPixmap;//站立时的照片
    QPixmap crouchingPixmap;//下蹲时的照片
    qreal standingHeight;//站立照片高度
    qreal crouchingHeight;//下蹲照片高度



};


#endif //QT_PROGRAMMING_2024_CHARACTER_H
