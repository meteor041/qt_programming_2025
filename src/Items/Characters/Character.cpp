//
// Created by gerw on 8/20/24.
//

#include <QTransform>
#include "Character.h"
#include "../Weapon/Fist.h"
#include <QDebug>

// 【核心修改 B1】修改构造函数
Character::Character(QGraphicsItem *parent)
    : Item(parent, ":/Biker_basic.png")
{
    // 隐藏基类创建的图形
    if (this->pixmapItem) {
        this->pixmapItem->setVisible(false);
    }

    // --- 【核心修改：加载所有需要的图片资源】 ---
    standingPixmap.load(":/Biker_basic.png");
    crouchingPixmap.load(":/Biker_crouch.png");

    // 2. 加载跑步动画（6帧）
    for (int i = 1; i < 7; ++i) {
        QString frameName = QString(":/Biker_run%1.png").arg(i);
        QPixmap frame(frameName);
        if (frame.isNull()) {
            qDebug() << "警告: 跑步动画帧未找到:" << frameName;
        } else {
            runningAnimationFrames.append(frame);
        }
    }
    if (runningAnimationFrames.isEmpty()) {
        qDebug() << "错误: 没有跑步动画帧被加载。";
        runningAnimationFrames.append(standingPixmap); // 备用
    }

    // 3. 加载跳跃动画（4帧）
    for (int i = 1; i < 5; ++i) {
        QString frameName = QString(":/Biker_jump%1.png").arg(i);
        QPixmap frame(frameName);
        if (frame.isNull()) {
            qDebug() << "警告: 跳跃动画帧未找到:" << frameName;
        } else {
            jumpingAnimationFrames.append(frame);
        }
    }
    if (jumpingAnimationFrames.isEmpty()) {
        qDebug() << "错误: 没有跳跃动画帧被加载。";
        jumpingAnimationFrames.append(standingPixmap); // 备用
    }
    // --- 【修改结束】 ---

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
    // 初始化默认武器：拳头
    weapon = new Fist(this);
    
    // 初始化肾上腺素效果相关变量
    adrenalineActive = false;
    adrenalineTimer = 0;
    adrenalineSpeedMultiplier = 1.0;
    adrenalineHealPerFrame = 0;
    adrenalineHealCounter = 0;
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

// --- 【核心改动 1】: 实现新增的公共方法 ---
void Character::setSpeedMultiplier(qreal multiplier) {
    m_speedMultiplier = multiplier;
}

void Character::setInStealth(bool stealth) {
    // 只有在状态改变时才打印，避免刷屏
    if (m_isInStealth != stealth) {
        m_isInStealth = stealth;
        qDebug() << "Character stealth status:" << m_isInStealth;
    }
}

Platform* Character::getCurrentPlatform() const {
    return m_currentPlatform;
}

void Character::setCurrentPlatform(Platform* platform) {
    m_currentPlatform = platform;
}


void Character::processInput() {
    // 0. 更新肾上腺素效果
    updateAdrenalineEffect();
    
    // 1. 处理拾取状态 (这必须在最前面，以决定本帧是否在执行拾取动作)
    if (!lastPickDown && pickDown) {
        picking = true;
    } else {
        picking = false;
    }
    lastPickDown = pickDown;

    // 更新角色状态和外观放到最后
    updateAppearanceAndState();


    // 3. 【重大改动】根据【新】的状态来决定行为
    QPointF currentVelocity = getVelocity();

    // 如果正在下蹲，则强制水平速度为0
    if (currentState == Crouching) {
        currentVelocity.setX(0);
    } else {
        // 只有在非下蹲状态下，才处理移动和跳跃
        const auto moveSpeed = 5.0;
        qreal targetHorizontalVelocity = 0;

        if (isLeftDown()) {
            // 应用平台速度倍率和肾上腺素速度倍率
            qreal totalSpeedMultiplier = m_speedMultiplier * (adrenalineActive ? adrenalineSpeedMultiplier : 1.0);
            targetHorizontalVelocity = -moveSpeed * totalSpeedMultiplier;
            setTransformOriginPoint(boundingRect().center());
            setTransform(QTransform().scale(-1, 1));
        } else if (isRightDown()) {
            // 应用平台速度倍率和肾上腺素速度倍率
            qreal totalSpeedMultiplier = m_speedMultiplier * (adrenalineActive ? adrenalineSpeedMultiplier : 1.0);
            targetHorizontalVelocity = moveSpeed * totalSpeedMultiplier;
            setTransformOriginPoint(boundingRect().center());
            setTransform(QTransform().scale(1, 1));
        }
        currentVelocity.setX(targetHorizontalVelocity);

        if (isJumpDown()) {
            const auto jumpImpulse = -18.0;
            currentVelocity.setY(jumpImpulse);
            setJumpDown(false); // 重置跳跃意图
        }
    }

    // 4. (保持不变) 设置最终速度
    setVelocity(currentVelocity);
}

void Character::updateAppearanceAndState() {
    // 1. 决定当前帧应该是什么状态
    CharacterState newState;
    if (!isOnGround()) {
        newState = Jumping;
    } else if (isCrouchDown() && !isPicking()) {
        newState = Crouching;
    } else if (velocity.x() != 0) {
        newState = Running;
    } else {
        newState = Standing;
    }

    // 2. 如果状态发生了变化，进行初始化处理
    if (newState != currentState) {
        // 处理从下蹲 -> 非下蹲的高度变化
        if (currentState == Crouching && newState != Crouching) {
            setY(y() - (standingHeight - crouchingHeight));
        }

        currentState = newState;
        animationFrameIndex = 0;
        animationFrameTimer = 0;

        // 【动画修复】在状态切换的瞬间，立即设置动画的第0帧
        switch (currentState) {
        case Standing:
            characterPixmapItem->setPixmap(standingPixmap);
            break;
        case Crouching:
            // 处理从非下蹲 -> 下蹲的高度变化
            setY(y() + (standingHeight - crouchingHeight));
            characterPixmapItem->setPixmap(crouchingPixmap);
            break;
        case Running:
            if (!runningAnimationFrames.isEmpty()) {
                characterPixmapItem->setPixmap(runningAnimationFrames[0]);
            }
            break;
        case Jumping:
            if (!jumpingAnimationFrames.isEmpty()) {
                characterPixmapItem->setPixmap(jumpingAnimationFrames[0]);
            }
            break;
        default:
            break;
        }
    }

    // 3. 根据当前状态，【每帧】更新动画
    animationFrameTimer++;

    // 重置crouching标志位，它只在Crouching状态下为true
    crouching = (currentState == Crouching);

    // 【动画修复】只在需要动画的状态下，才处理帧更新逻辑
    switch (currentState) {
    case Running:
        if (animationFrameTimer >= ANIMATION_FRAME_DURATION && !runningAnimationFrames.isEmpty()) {
            animationFrameTimer = 0;
            animationFrameIndex = (animationFrameIndex + 1) % runningAnimationFrames.size();
            characterPixmapItem->setPixmap(runningAnimationFrames[animationFrameIndex]);
        }
        break;
    case Jumping:
        if (animationFrameTimer >= ANIMATION_FRAME_DURATION && !jumpingAnimationFrames.isEmpty()) {
            animationFrameTimer = 0;
            animationFrameIndex = (animationFrameIndex + 1) % jumpingAnimationFrames.size();
            characterPixmapItem->setPixmap(jumpingAnimationFrames[animationFrameIndex]);
        }
        break;
    // Standing 和 Crouching 不需要每帧更新动画
    case Standing:
    case Crouching:
        break;
    }
    if (m_isInStealth) {
        // 如果处于隐身状态，设置半透明效果
        this->setOpacity(0.4); // 0.2可能太透明了，可以调整为0.4试试
    } else {
        // 否则，恢复完全不透明
        this->setOpacity(1.0);
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

// 武器相关方法实现
Weapon* Character::getWeapon() const {
    return weapon;
}

void Character::setWeapon(Weapon* newWeapon) {
    QPointF oldWeaponPos;
    if (weapon != nullptr && weapon != newWeapon) {
        // 卸载当前武器
        oldWeaponPos = weapon->pos();
        weapon->setPos(newWeapon->pos());
        weapon->unmount();
        weapon->setParentItem(parentItem());
    }
    
    if (newWeapon != nullptr) {
        // 装备新武器
        if (weapon != nullptr) {
            newWeapon->setPos(oldWeaponPos);
        }
        newWeapon->setParentItem(this);
        newWeapon->mountToParent();
    }
    weapon = newWeapon;
}

// 在现有的武器相关方法后添加以下实现

void Character::performAttack() {
    if (weapon != nullptr) {
        weapon->attack(this);
    } else {
        qDebug() << "No weapon equipped, cannot attack!";
    }
}

// 新增：获取所持武器攻击范围的函数
qreal Character::getWeaponAttackRange() const {
    if (weapon != nullptr) {
        return weapon->getAttackRange();
    } else {
        // 如果没有武器，返回默认的近身攻击范围
        qDebug() << "No weapon equipped, using default attack range";
        return 50.0; // 默认近身攻击范围50像素
    }
}

// 在文件末尾添加以下方法实现

// 生命值系统实现
int Character::getHealth() const {
    return health;
}

int Character::getMaxHealth() const {
    return maxHealth;
}

bool Character::isDead() const {
    return dead;
}

void Character::setHealth(int health) {
    this->health = qBound(0, health, maxHealth);
    if (this->health <= 0) {
        dead = true;
        this->health = 0;
    } else {
        dead = false;
    }
}

void Character::takeDamage(int damage) {
    if (dead) return; // 已死亡则不再受伤
    
    health -= damage;
    if (health <= 0) {
        health = 0;
        dead = true;
        qDebug() << "Character died!";
    }
    qDebug() << "Character took" << damage << "damage, health:" << health;
}

void Character::heal(int amount) {
    if (dead) return; // 已死亡则无法治疗
    
    health = qMin(health + amount, maxHealth);
    qDebug() << "Character healed" << amount << "health, current health:" << health;
}

// 肾上腺素效果系统实现
void Character::startAdrenalineEffect(int duration, qreal speedMultiplier, int healPerFrame) {
    adrenalineActive = true;
    adrenalineTimer = duration;
    adrenalineSpeedMultiplier = speedMultiplier;
    adrenalineHealPerFrame = healPerFrame;
    adrenalineHealCounter = 0;
    
    qDebug() << "Adrenaline effect started: duration =" << duration 
             << ", speed multiplier =" << speedMultiplier 
             << ", heal per frame =" << healPerFrame;
}

void Character::updateAdrenalineEffect() {
    if (!adrenalineActive) return;
    
    // 减少剩余时间
    adrenalineTimer--;
    
    // 处理持续回血（每60帧回血一次）
    adrenalineHealCounter++;
    if (adrenalineHealCounter >= 60) {
        heal(adrenalineHealPerFrame);
        adrenalineHealCounter = 0;
    }
    
    // 检查效果是否结束
    if (adrenalineTimer <= 0) {
        adrenalineActive = false;
        adrenalineSpeedMultiplier = 1.0;
        qDebug() << "Adrenaline effect ended";
    }
}

