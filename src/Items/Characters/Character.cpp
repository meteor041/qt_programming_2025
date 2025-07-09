//
// Created by gerw on 8/20/24.
//

#include <QTransform>
#include "Character.h"

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

    // 1. 获取当前速度，我们不再重置它，而是在它的基础上修改
    QPointF currentVelocity = getVelocity();

    // 2. 处理水平移动
    const auto moveSpeed = 5.0; // 我们可以给一个稍大的瞬时速度
    qreal targetHorizontalVelocity = 0;
    if (isLeftDown()) {
        targetHorizontalVelocity = -moveSpeed;
        setTransformOriginPoint(boundingRect().center());
        setTransform(QTransform().scale(-1,1));
    } else if (isRightDown()) {
        targetHorizontalVelocity = moveSpeed;
        setTransformOriginPoint(boundingRect().center());
        setTransform(QTransform().scale(1,1));
    }
    currentVelocity.setX(targetHorizontalVelocity);

    // 3. 处理跳跃 - 这是一个瞬时动作
    // 只有当跳跃键被按下的那一帧，我们才施加向上的速度脉冲。
    // BattleScene 会确保只有在地面上时 setJumpDown(true) 才有效。
    if (isJumpDown()) {
        const auto jumpImpulse = -18.0; // 负数表示向上，这是一个较强的瞬时速度
        currentVelocity.setY(jumpImpulse);
        // 跳跃是一次性的，施加完脉冲后就将状态重置，防止连续跳跃
        setJumpDown(false);
    }

    // 4. 将计算出的新速度设置回去
    setVelocity(currentVelocity);

    // 5. 处理拾取逻辑 (保持不变)
    // 调试输出

    // qDebug() << "Character velocity:" << velocity;

    if (!lastPickDown && pickDown) { // first time pickDown
        picking = true;
    } else {
        picking = false;
    }
    lastPickDown = pickDown;
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

