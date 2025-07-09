//
// Created by gerw on 8/20/24.
//

#ifndef QT_PROGRAMMING_2024_CHARACTER_H
#define QT_PROGRAMMING_2024_CHARACTER_H

#include <QGraphicsEllipseItem>
#include "../HeadEquipments/HeadEquipment.h"
#include "../Armors/Armor.h"
#include "../LegEquipments/LegEquipment.h"

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
    bool leftDown{}, rightDown{}, pickDown{}, jumpDown{}, onGround{};
    bool lastPickDown{};
    bool picking{};
    
    // 跳跃计时器相关变量
    int jumpCooldownTimer{0};        // 跳跃冷却计时器
    static const int JUMP_COOLDOWN_FRAMES = 30; // 跳跃冷却帧数（约0.33秒，假设90FPS）
};


#endif //QT_PROGRAMMING_2024_CHARACTER_H
