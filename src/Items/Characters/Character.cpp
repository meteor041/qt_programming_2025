// src/Items/Characters/Character.cpp

#include "Character.h"
#include <QPainter>
#include <QDateTime>
//#include "../Weapons/Fist.h"

constexpr qreal MOVE_SPEED = 350.0;
constexpr qreal JUMP_STRENGTH = 950.0;
// [NEW] 动画帧的尺寸 (请根据你的图片实际尺寸进行调整!)
constexpr int FRAME_WIDTH = 48;
constexpr int FRAME_HEIGHT = 48;

Character::Character(int playerID, QGraphicsItem *parent)
    : QGraphicsObject(parent),
    playerID(playerID),
    health(100), maxHealth(100),
    facingRight(true), isOnGround(false), isCrouching(false),
//    headEquipment(nullptr), legEquipment(nullptr), armor(nullptr), currentWeapon(nullptr),
    velocity(0, 0),
    leftDown(false), rightDown(false), crouchDown(false), jumpDown(false), fireDown(false),
    lastCrouchDown(false), pickingIntent(false),
    currentState(IDLE), currentFrame(0), frameTimer(0)
{
    // 默认装备拳头
//    currentWeapon = new Fist(this);
//    currentWeapon->mountToParent();

    // 加载所有动画资源 (请确保你已在.qrc文件中添加了这些图片)
    animations[IDLE]      = loadAnimationFromSheet(":/Biker_idle.png", FRAME_WIDTH, FRAME_HEIGHT, 150, true);
    animations[RUNNING]   = loadAnimationFromSheet(":/Biker_run.png", FRAME_WIDTH, FRAME_HEIGHT, 100, true);
    animations[JUMPING]   = loadAnimationFromSheet(":/Biker_jump.png", FRAME_WIDTH, FRAME_HEIGHT, 120, false);
    animations[CROUCHING] = loadAnimationFromSheet(":/Biker_crouch.png", FRAME_WIDTH, FRAME_HEIGHT, 200, false); // 假设有下蹲图
    animations[HURT]      = loadAnimationFromSheet(":/Biker_hurt.png", FRAME_WIDTH, FRAME_HEIGHT, 200, false);
    animations[DEATH]     = loadAnimationFromSheet(":/Biker_death.png", FRAME_WIDTH, FRAME_HEIGHT, 150, false);
    animations[PUNCHING]  = loadAnimationFromSheet(":/Biker_punch.png", FRAME_WIDTH, FRAME_HEIGHT, 80, false);

    setState(IDLE); // 设置初始动画
}

Character::~Character() { }

Animation Character::loadAnimationFromSheet(const QString& filePath, int frameWidth, int frameHeight, qreal frameDuration, bool loops) {
    Animation anim;
    anim.frameDuration = frameDuration;
    anim.loops = loops;
    QPixmap sheet(filePath);
    for (int x = 0; x < sheet.width(); x += frameWidth) {
        anim.frames.append(sheet.copy(x, 0, frameWidth, frameHeight));
    }
    return anim;
}

QRectF Character::boundingRect() const {
    // 返回一个足够大的矩形来包含所有可能的帧
    return {-FRAME_WIDTH/2.0, -FRAME_HEIGHT, (qreal)FRAME_WIDTH, (qreal)FRAME_HEIGHT};
}

void Character::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (animations.isEmpty() || !animations.contains(currentState) || animations[currentState].frames.isEmpty()) {
        return; // 防止在资源未加载时崩溃
    }
    const QPixmap& frame = animations[currentState].frames[currentFrame];
    painter->save();
    if (!facingRight) {
        painter->scale(-1, 1);
    }
    // 绘制，以角色脚底中心为原点
    painter->drawPixmap(-frame.width() / 2.0, -frame.height(), frame);
    painter->restore();
}

void Character::update(qint64 deltaTime) {
    processInput();

    // 应用速度更新位置
    setPos(pos() + velocity * (deltaTime / 1000.0));

    // 更新拾取意图
    pickingIntent = (!lastCrouchDown && crouchDown);
    lastCrouchDown = crouchDown;

    // 更新动画状态
    State newState = currentState;
    if (health <= 0) {
        newState = DEATH;
    } else if (isCrouching) {
        newState = CROUCHING;
    } else if (!isOnGround) {
        newState = JUMPING;
    } else if (velocity.x() != 0) {
        newState = RUNNING;
    } else {
        newState = IDLE;
    }
    // 非循环动画（如攻击、受伤）会覆盖上述状态，并在播放完毕后自动切换
    if (currentState == PUNCHING || currentState == HURT) {
        if (frameTimer >= animations[currentState].frameDuration * animations[currentState].frames.size()) {
            // 动画播完了，切回默认状态
            setState(IDLE);
        } else {
            // 动画还在播，不改变状态
        }
    } else {
        setState(newState);
    }


    // 更新动画帧
    frameTimer += deltaTime;
    const Animation& currentAnim = animations[currentState];
    if (currentAnim.frameDuration > 0 && frameTimer >= currentAnim.frameDuration) {
        frameTimer = fmod(frameTimer, currentAnim.frameDuration);
        currentFrame++;
        if (currentFrame >= currentAnim.frames.size()) {
            currentFrame = currentAnim.loops ? 0 : currentAnim.frames.size() - 1;
        }
    }
}

void Character::setState(State newState) {
    if (currentState == newState) return;
    currentState = newState;
    currentFrame = 0;
    frameTimer = 0;
}

void Character::processInput() {
    isCrouching = crouchDown && isOnGround;

    if (!isCrouching) {
        if (leftDown) { velocity.setX(-MOVE_SPEED); facingRight = false; }
        else if (rightDown) { velocity.setX(MOVE_SPEED); facingRight = true; }
        else { velocity.setX(0); }
    } else {
        velocity.setX(0);
    }

    if (jumpDown && isOnGround && !isCrouching) {
        velocity.setY(-JUMP_STRENGTH);
        setIsOnGround(false);
    }

    if (fireDown) { fire(); }
}

void Character::fire() {
 //   if (currentWeapon && (currentState == IDLE || currentState == RUNNING)) { // 只有在特定状态下才能攻击
//        setState(PUNCHING);
        // 实际攻击判定可以在动画的某一帧触发
        // currentWeapon->attack(this, facingRight);
//    }
}

void Character::takeDamage(int amount) {
    if (health <= 0) return;
    health -= amount;
    setState(HURT);
    if (health <= 0) {
        health = 0;
        emit died(this);
    }
}

// --- 其他函数的实现 ---
void Character::heal(int amount) { health = qMin(maxHealth, health + amount); }
void Character::setCrouchDown(bool down) { crouchDown = down; }
void Character::setLeftDown(bool down) { leftDown = down; }
void Character::setRightDown(bool down) { rightDown = down; }
void Character::setJumpDown(bool down) { jumpDown = down; }
void Character::setFireDown(bool down) { fireDown = down; }
const QPointF &Character::getVelocity() const { return velocity; }
void Character::setVelocity(const QPointF &newVelocity) { velocity = newVelocity; }
void Character::setVelocity(qreal vx, qreal vy) { velocity.setX(vx); velocity.setY(vy); }
void Character::addVelocity(const QPointF &dv) { velocity += dv; }
void Character::setIsOnGround(bool ground) { isOnGround = ground; if (isOnGround && velocity.y() > 0) velocity.setY(0); }
bool Character::isPicking() const { return pickingIntent && isOnGround; }

Armor *Character::pickupArmor(Armor *newArmor) {
    // ... 实现和之前一样 ...
    return nullptr;
}
/*Weapon *Character::pickupWeapon(Weapon *newWeapon) {
    // ... 实现和之前一样 ...
    return nullptr;
}*/
