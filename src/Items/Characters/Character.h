// src/Items/Characters/Character.h

#ifndef QT_PROGRAMMING_2024_CHARACTER_H
#define QT_PROGRAMMING_2024_CHARACTER_H

#include <QGraphicsObject>
#include <QPointF>
#include <QMap>
#include <QVector>
#include "../Armors/Armor.h"
//#include "../Weapons/Weapon.h"

class HeadEquipment;
class LegEquipment;

// [NEW] 动画数据结构
struct Animation {
    QVector<QPixmap> frames;
    qreal frameDuration; // ms
    bool loops;
};

class Character : public QGraphicsObject {
    Q_OBJECT

public:
    // [NEW] 角色状态机
    enum State {
        IDLE,
        RUNNING,
        JUMPING,
        CROUCHING,
        PUNCHING,
        HURT,
        DEATH
    };

    explicit Character(int playerID, QGraphicsItem *parent = nullptr);
    ~Character() override;

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void update(qint64 deltaTime);

    // --- 输入处理与状态设置 ---
    void setLeftDown(bool down);
    void setRightDown(bool down);
    void setCrouchDown(bool down);
    void setJumpDown(bool down);
    void setFireDown(bool down);

    // --- 物理与运动 ---
    [[nodiscard]] const QPointF &getVelocity() const;
    void setVelocity(const QPointF &newVelocity);
    void setVelocity(qreal vx, qreal vy);
    void addVelocity(const QPointF &dv);
    void setIsOnGround(bool ground);

    // --- 战斗与交互 ---
    void takeDamage(int amount);
    void heal(int amount);
    bool isPicking() const;
    Armor* pickupArmor(Armor* newArmor);
 //   Weapon* pickupWeapon(Weapon* newWeapon);

signals:
    void died(Character* character);

protected:
    void processInput();
    void fire();
    void setState(State newState);

    // --- 核心属性与状态 ---
    int playerID;
    int health;
    int maxHealth;
    bool facingRight;
    bool isOnGround;
    bool isCrouching;

    // --- 装备 ---
    HeadEquipment *headEquipment{};
    LegEquipment *legEquipment{};
    Armor *armor{};
 //   Weapon *currentWeapon{};

    // --- 物理 ---
    QPointF velocity{};

private:
    // --- 动画系统 ---
    State currentState;
    QMap<State, Animation> animations;
    int currentFrame;
    qreal frameTimer;
    Animation loadAnimationFromSheet(const QString& filePath, int frameWidth, int frameHeight, qreal frameDuration, bool loops);

    // --- 输入状态 ---
    bool leftDown{}, rightDown{}, crouchDown{}, jumpDown{}, fireDown{};
    bool lastCrouchDown{};
    bool pickingIntent{};
};

#endif //QT_PROGRAMMING_2024_CHARACTER_H
