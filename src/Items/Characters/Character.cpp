//
// Created by gerw on 8/20/24.
//

#include <QTransform>
#include "Character.h"
#include <QDebug>

// 【核心修改 B1】修改构造函数
Character::Character(QGraphicsItem *parent)
    // 1. 调用基类构造函数，给它一个默认的、将被隐藏的图片
    : Item(parent, ":/Biker_basic.png")
{
    // 2. 隐藏基类创建的图形
    if (this->pixmapItem) { // this->pixmapItem 是从 Item 继承来的
        this->pixmapItem->setVisible(false);
    }

    // 3. 加载 Character 自己需要的图片资源
    standingPixmap.load(":/Biker_basic.png");
    crouchingPixmap.load(":/Biker_crouch.png");

    if (crouchingPixmap.isNull()) {
        qDebug() << "Warning: Crouching pixmap not found.";
        crouchingPixmap = standingPixmap;
    }

    // 4. 缓存高度信息
    standingHeight = standingPixmap.height();
    crouchingHeight = crouchingPixmap.height();

    // 5. 创建并管理 Character 自己的图形项
    characterPixmapItem = new QGraphicsPixmapItem(this); // 设为this的子项
    characterPixmapItem->setPixmap(standingPixmap);     // 设置初始外观

    // (调试用椭圆，保持不变)
    ellipseItem = new QGraphicsEllipseItem(-5, -5, 10, 10, this);
    ellipseItem->setBrush(Qt::green);
    ellipseItem->setZValue(1);
}

QRectF Character::boundingRect() const {
    // 返回我们自己管理的、可见的图形项的边界
    if (characterPixmapItem) {
        return characterPixmapItem->boundingRect();
    }
    // 作为备用，如果自己的图形项不存在，就返回一个空矩形
    return {};
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
    bool shouldCrouch = isCrouchDown() && !isPicking();

    if (shouldCrouch && !crouching) {
        crouching = true;
        // 操作我们自己的 characterPixmapItem
        characterPixmapItem->setPixmap(crouchingPixmap);
        setY(y() + (standingHeight - crouchingHeight));
    } else if (!shouldCrouch && crouching) {
        crouching = false;
        // 操作我们自己的 characterPixmapItem
        characterPixmapItem->setPixmap(standingPixmap);
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

