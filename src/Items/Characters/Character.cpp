//
// Created by gerw on 8/20/24.
//

#include <QTransform>
#include "Character.h"
#include <QDebug>

Character::Character(QGraphicsItem *parent) : Item(parent, ":/Biker_basic.png") {
   ellipseItem = new QGraphicsEllipseItem(-5, -5, 10, 10, this);
   // Optionally, set some properties of the ellipse
   ellipseItem->setBrush(Qt::green);          // Fill color
   ellipseItem->setZValue(1);
}

bool Character::isLeftDown() const {
    return leftDown;
}

void Character::setLeftDown(bool leftDown) {
    Character::leftDown = leftDown;
}

bool Character::isRightDown() const {
    return rightDown;
}

void Character::setRightDown(bool rightDown) {
    Character::rightDown = rightDown;
}


void Character::setJumpDown(bool jumpDown) {
    Character::jumpDown = jumpDown;
}

bool Character::isJumpDown() const {
    return jumpDown;
}

bool Character::isPickDown() const {
    return pickDown;
}

void Character::setOnGround(bool onGround) {
    Character::onGround = onGround;
}

bool Character::isOnGround() const {
    return onGround;
}

bool Character::isCrouchDown() const {
    return crouchDown;
}

void Character::setCrouchDown(bool crouchDown) {
    Character::crouchDown = crouchDown;
}

bool Character::isCrouching() const {
    return crouching;
}

void Character::setPickDown(bool pickDown) {
    Character::pickDown = pickDown;
}

const QPointF &Character::getVelocity() const {
    return velocity;
}

void Character::setVelocity(const QPointF &velocity) {
    Character::velocity = velocity;
}

void Character::processInput() {
    // 1. 处理拾取状态 (这必须在最前面，以决定本帧是否在执行拾取动作)
    if (!lastPickDown && pickDown) {
        picking = true;
    } else {
        picking = false;
    }
    lastPickDown = pickDown;

    // 2. 更新角色状态和外观 (下蹲/站立)
    updateAppearanceAndState();

    // 3. 如果正在下蹲，则禁止移动和跳跃，直接返回
    if (isCrouching()) {
        // 将水平速度清零
        velocity.setX(0);
        return; // 直接结束，不处理后续的移动和跳跃输入
    }

    // 4. (如果不下蹲) 处理水平移动
    QPointF currentVelocity = getVelocity();
    const auto moveSpeed = 5.0;
    qreal targetHorizontalVelocity = 0;
    if (isLeftDown()) {
        targetHorizontalVelocity = -moveSpeed;
        setTransformOriginPoint(boundingRect().center());
        setTransform(QTransform().scale(-1, 1));
    } else if (isRightDown()) {
        targetHorizontalVelocity = moveSpeed;
        setTransformOriginPoint(boundingRect().center());
        setTransform(QTransform().scale(1, 1));
    }
    currentVelocity.setX(targetHorizontalVelocity);

    // 5. (如果不下蹲) 处理跳跃
    if (isJumpDown()) {
        const auto jumpImpulse = -18.0;
        currentVelocity.setY(jumpImpulse);
        setJumpDown(false);
    }

    setVelocity(currentVelocity);
}

void Character::updateAppearanceAndState() {
    // 决定是否应该下蹲：当S键按下，并且本帧没有触发拾取动作时
    bool shouldCrouch = isCrouchDown() && !isPicking();

    // 状态发生变化时，更新外观和位置
    if (shouldCrouch && !crouching) {
        // 从 站立 -> 下蹲
        crouching = true;
//        setPixmap(crouchingPixmap);
        // 为了让脚底位置不变，身体需要向下移动 高度差 的距离
        setY(y() + (standingHeight - crouchingHeight));
    } else if (!shouldCrouch && crouching) {
        // 从 下蹲 -> 站立
        crouching = false;
//        setPixmap(standingPixmap);
        // 恢复站立时，身体需要向上移动
        setY(y() - (standingHeight - crouchingHeight));
    }
}

bool Character::isPicking() const {
    return picking;
}

Armor *Character::pickupArmor(Armor *newArmor) {
    auto oldArmor = armor;
    if (oldArmor != nullptr) {
        oldArmor->unmount();
        oldArmor->setPos(newArmor->pos());
        oldArmor->setParentItem(parentItem());
    }
    newArmor->setParentItem(this);
    newArmor->mountToParent();
    armor = newArmor;
    return oldArmor;
}

