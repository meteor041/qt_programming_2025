//
// Created by gerw on 8/20/24.
//

#include <QTransform>
#include "Character.h"
#include "../Weapon/Fist.h"
#include <QDebug>
#include <QGraphicsScene>

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

    // 【新增】加载攻击动画（假设有6帧）
    for (int i = 1; i < 7; ++i) {
        QString frameName = QString(":/Biker_attack%1.png").arg(i);
        QPixmap frame(frameName);
        if (frame.isNull()) {
            qDebug() << "警告: 攻击动画帧未找到:" << frameName;
        } else {
            attackingAnimationFrames.append(frame);
        }
    }
    if (attackingAnimationFrames.isEmpty()) {
        qDebug() << "错误: 没有攻击动画帧被加载。";
        attackingAnimationFrames.append(standingPixmap); // 备用
    }

    // 【新增】加载受击动画（假设有2帧）
    for (int i = 1; i < 3; ++i) {
        QString frameName = QString(":/Biker_hit%1.png").arg(i);
        QPixmap frame(frameName);
        if (frame.isNull()) {
            qDebug() << "警告: 受击动画帧未找到:" << frameName;
        } else {
            hitAnimationFrames.append(frame);
        }
    }
    if (hitAnimationFrames.isEmpty()) {
        qDebug() << "错误: 没有受击动画帧被加载。";
        hitAnimationFrames.append(standingPixmap); // 备用
    }

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

    // --- 【问题修复】在这里一次性设置变换原点 ---
    // 设置变换原点为图形的中心。这确保了翻转是围绕中心进行的。
    // 这个设置只需要在构造函数中执行一次。
    setTransformOriginPoint(characterPixmapItem->boundingRect().center());
    // --- 修复结束 ---

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

// ... (其他 getter/setter 函数保持不变) ...
bool Character::isLeftDown() const { return leftDown; }
void Character::setLeftDown(bool leftDown) { Character::leftDown = leftDown; }
bool Character::isRightDown() const { return rightDown; }
void Character::setRightDown(bool rightDown) { Character::rightDown = rightDown; }
void Character::setJumpDown(bool jumpDown) { Character::jumpDown = jumpDown; }
bool Character::isJumpDown() const { return jumpDown; }
bool Character::isPickDown() const { return pickDown; }
void Character::setOnGround(bool onGround) { Character::onGround = onGround; }
bool Character::isOnGround() const { return onGround; }
bool Character::isCrouchDown() const { return crouchDown; }
void Character::setCrouchDown(bool crouchDown) { Character::crouchDown = crouchDown; }
bool Character::isCrouching() const { return crouching; }
void Character::setPickDown(bool pickDown) { Character::pickDown = pickDown; }
const QPointF &Character::getVelocity() const { return velocity; }
void Character::setVelocity(const QPointF &velocity) { Character::velocity = velocity; }
void Character::setSpeedMultiplier(qreal multiplier) { m_speedMultiplier = multiplier; }
void Character::setInStealth(bool stealth) { if (m_isInStealth != stealth) { m_isInStealth = stealth; qDebug() << "Character stealth status:" << m_isInStealth; } }
Platform* Character::getCurrentPlatform() const { return m_currentPlatform; }
void Character::setCurrentPlatform(Platform* platform) { m_currentPlatform = platform; }


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

    // 如果正在受击，则不允许移动
    if (hitStateTimer > 0) {
        currentVelocity.setX(0);
    }

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
            // --- 【问题修复】删除此处的 setTransformOriginPoint ---
            // setTransformOriginPoint(boundingRect().center());
            setTransform(QTransform().scale(-1, 1));
            // 更新朝向：向左移动时facingRight为false
            setFacingRight(false);
        } else if (isRightDown()) {
            // 应用平台速度倍率和肾上腺素速度倍率
            qreal totalSpeedMultiplier = m_speedMultiplier * (adrenalineActive ? adrenalineSpeedMultiplier : 1.0);
            targetHorizontalVelocity = moveSpeed * totalSpeedMultiplier;
            // --- 【问题修复】删除此处的 setTransformOriginPoint ---
            // setTransformOriginPoint(boundingRect().center());
            setTransform(QTransform().scale(1, 1));
            // 更新朝向：向右移动时facingRight为true
            setFacingRight(true);
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

// ... (文件剩余部分保持不变) ...
void Character::updateAppearanceAndState() {
    // 1. 【核心修改】最优先处理受击状态的计时和恢复
    if (hitStateTimer > 0) {
        hitStateTimer--; // 每帧减少计时

        // 确保在受击状态时显示的是第二张图
        if (hitAnimationFrames.size() > 1) {
            characterPixmapItem->setPixmap(hitAnimationFrames.at(1));
        }

        if (hitStateTimer <= 0) {
            // 受击状态结束，强制恢复到站立状态
            currentState = Standing;
            characterPixmapItem->setPixmap(standingPixmap);
            // 确保动画参数被重置
            animationFrameIndex = 0;
            animationFrameTimer = 0;
        }

        // 如果仍在受击状态，则直接返回，不执行下面的任何逻辑
        if (m_isInStealth) this->setOpacity(0.4); else this->setOpacity(1.0);
        return;
    }

    // 2. 处理一次性动画（现在只剩下攻击）
    if (currentState == Attacking) {
        animationFrameTimer++;
        QList<QPixmap>* currentAnimation = &attackingAnimationFrames;

        if (animationFrameTimer >= ANIMATION_FRAME_DURATION && !currentAnimation->isEmpty()) {
            animationFrameTimer = 0;
            animationFrameIndex++;

            // 检查动画是否播放完毕
            if (animationFrameIndex >= currentAnimation->size()) {
                // 动画结束，恢复到之前的状态
                currentState = previousState;
                animationFrameIndex = 0;
            } else {
                // 继续播放动画
                characterPixmapItem->setPixmap(currentAnimation->at(animationFrameIndex));
            }
        }
        // 如果正在播放一次性动画，直接返回
        if (currentState == Attacking) {
            if (m_isInStealth) this->setOpacity(0.4); else this->setOpacity(1.0);
            return;
        }
    }

    // 2. 决定常规状态 (如果不在播放一次性动画)
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

    // 3. 如果状态发生变化，进行初始化处理
    if (newState != currentState) {
        if (currentState == Crouching && newState != Crouching) {
            setY(y() - (standingHeight - crouchingHeight));
        }

        currentState = newState;
        animationFrameIndex = 0;
        animationFrameTimer = 0;

        switch (currentState) {
        case Standing:
            characterPixmapItem->setPixmap(standingPixmap);
            break;
        case Crouching:
            setY(y() + (standingHeight - crouchingHeight));
            characterPixmapItem->setPixmap(crouchingPixmap);
            break;
        case Running:
            if (!runningAnimationFrames.isEmpty()) characterPixmapItem->setPixmap(runningAnimationFrames[0]);
            break;
        case Jumping:
            if (!jumpingAnimationFrames.isEmpty()) characterPixmapItem->setPixmap(jumpingAnimationFrames[0]);
            break;
        // Attacking 和 Hit 状态的启动不在这里处理，而是在触发函数中
        default:
            break;
        }
    }

    // 4. 根据当前状态，【每帧】更新循环动画 (跑步/跳跃)
    animationFrameTimer++;
    crouching = (currentState == Crouching);

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
    default:
        break;
    }

    // 透明度效果
    if (m_isInStealth) {
        this->setOpacity(0.4);
    } else {
        this->setOpacity(1.0);
    }
}


bool Character::isPicking() const {
    return picking;
}

Armor *Character::pickupArmor(Armor *newArmor) {
    Armor *oldArmor = armor;
    if (oldArmor != nullptr) {
        oldArmor->unmount();
        // 将旧护甲放在新护甲原来的位置
        if(newArmor) {
            oldArmor->setPos(newArmor->pos());
        }
        oldArmor->setParentItem(parentItem());
    }
    if (newArmor != nullptr) {
        newArmor->setParentItem(this);
        newArmor->mountToParent();
    }
    armor = newArmor;
    return oldArmor; // 返回旧护甲，场景可以继续管理它
}

// 武器相关方法实现
Weapon* Character::getWeapon() const {
    return weapon;
}

Armor* Character::getArmor() const {
    return armor;
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
    // 如果正在播放其他一次性动画，则不允许攻击，避免动画冲突
    if (currentState == Attacking || currentState == Hit) {
        return;
    }

    // 触发攻击动画
    if (!attackingAnimationFrames.isEmpty()) {
        previousState = currentState; // 保存当前状态
        currentState = Attacking;
        animationFrameIndex = 0;
        animationFrameTimer = 0;
        characterPixmapItem->setPixmap(attackingAnimationFrames[0]); //立即显示第一帧
    }

    // 武器攻击逻辑（保持不变）
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

qreal Character::getWeaponAttackPower() const {
    if (weapon != nullptr) {
        return weapon->getAttackPower();
    } else {
        // 如果没有武器，返回默认的近身攻击范围
        qDebug() << "No weapon equipped, using default attack power";
        return 1; // 默认攻击力为1
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

// 【修改】takeDamage函数以处理护甲逻辑
void Character::takeDamage(int damage, Weapon* sourceWeapon) {
    if (dead) return;

    int finalDamage = damage;

    // 如果装备了护甲，则先由护甲处理伤害
    if (armor != nullptr) {
        qDebug() << "Armor is processing damage...";
        finalDamage = armor->processDamage(damage, sourceWeapon);

        // 检查护甲是否在此次伤害后损坏
        if (armor->isBroken()) {
            qDebug() << "Armor broke and is being removed.";
            // 从场景中移除并删除护甲对象
            // 注意：直接删除可能会有问题，更好的做法是标记它，
            // 然后在场景的主循环中安全地删除。
            // 但为简化，我们先直接处理。
            if(this->scene()){
                this->scene()->removeItem(armor);
            }
            delete armor;
            armor = nullptr;
        }
    }

    // 如果仍有伤害，触发受击动画
    if (finalDamage > 0) {
        // 在进入受击状态前，检查是否需要从下蹲状态中恢复
        if (currentState == Crouching) {
            // 如果当前是下蹲状态，必须先恢复Y坐标，否则会掉下去！
            setY(y() - (standingHeight - crouchingHeight));
            crouching = false; // 确保下蹲状态被完全重置
        }
        //
        // 【核心修改】启动受击状态计时器
        hitStateTimer = 30; // 设置或重置受击状态持续90帧
        currentState = Hit; // 立即进入受击状态，这会阻止其他动画播放
        animationFrameIndex = 0; // 重置动画帧，以防万一
        animationFrameTimer = 0;

        // 【核心修改】这里不再播放动画，而是在updateAppearanceAndState中强制显示第二张图
    }

    // 角色承受最终伤害
    health -= finalDamage;
    if (health <= 0) {
        health = 0;
        dead = true;
        qDebug() << "Character died!";
    }
    qDebug() << "Character took" << finalDamage << "final damage, health:" << health;
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

// 朝向相关方法实现
bool Character::isFacingRight() const {
    return facingRight;
}

void Character::setFacingRight(bool facingRight) {
    this->facingRight = facingRight;
}
