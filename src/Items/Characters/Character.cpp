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
    auto velocity = QPointF(0, 0);
    const auto moveSpeed = 0.3;
    if (isLeftDown()) {
        velocity.setX(velocity.x() - moveSpeed);
        setTransform(QTransform().scale(1, 1));
    } else if (isRightDown()) {
        velocity.setX(velocity.x() + moveSpeed);
        setTransform(QTransform().scale(-1, 1));
    }
    if (jumpDown) {
        velocity.setX(Character::velocity.x()); // 左右速度不变
        velocity.setY(velocity.y() - 4);
    } 
    if (!onGround) {
        velocity.setY(Character::velocity.y() + 0.1);
    } else if (!jumpDown) {
        velocity.setY(0);
    }
    setVelocity(velocity);

    // 调试输出
    qDebug() << "Character velocity:" << velocity;
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

