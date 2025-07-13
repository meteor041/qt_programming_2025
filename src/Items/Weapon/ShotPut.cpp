#include "ShotPut.h"
#include "../Characters/Character.h"
#include "../../Scenes/BattleScene.h"
#include <QDebug>
#include <QPixmap>
#include <QGraphicsScene>
#include <QtMath>
#include "Fist.h"  // 添加这个包含


// 实心球武器构造函数
ShotPut::ShotPut(QGraphicsItem *parent, int throwCount)
    : QObject(nullptr),Weapon(parent, QString::number(ShotPut::DAMAGE)), maxThrows(throwCount), remainingThrows(throwCount) {

    // 设置攻击范围（投掷距离）
    attackRange = 300.0;

    // 设置实心球图片
    if (!pixmapItem) {
        pixmapItem = new QGraphicsPixmapItem(this);
    }
    if (pixmapItem) {
        // 创建一个简单的圆形图片代表实心球
        QPixmap shotPutPixmap(20, 20);
        shotPutPixmap.fill(Qt::magenta);
        pixmapItem->setPixmap(shotPutPixmap);
        pixmapItem->setPos(25, -5);
    }

    qDebug() << "ShotPut created with" << maxThrows << "throws";
}

// 添加 ShotPut 析构函数
ShotPut::~ShotPut() {
    qDebug() << "ShotPut destructor called, clearing" << activeProjectiles.size() << "projectiles";

    // 通知所有活跃的投掷物：武器已被销毁
    for (ShotPutProjectile* projectile : activeProjectiles) {
        if (projectile) {
            projectile->clearSourceWeapon();
        }
    }
    activeProjectiles.clear();
}

// 添加注册/注销方法
void ShotPut::registerProjectile(ShotPutProjectile* projectile) {
    if (projectile && !activeProjectiles.contains(projectile)) {
        activeProjectiles.append(projectile);
    }
}

void ShotPut::unregisterProjectile(ShotPutProjectile* projectile) {
    activeProjectiles.removeAll(projectile);
}

// 重写攻击方法
void ShotPut::attack(Character *attacker) {
    qDebug() << "ShotPut attack";
    if (attacker == nullptr) {
        qDebug() << "Warning: Attacker is null, cannot throw shot put";
        return;
    }

    // 检查是否已装备
    if (!isMounted()) {
        qDebug() << "Warning: ShotPut is not equipped, cannot throw";
        return;
    }

    // 检查是否还有投掷次数
    if (!canThrow()) {
        qDebug() << "ShotPut is out of throws!";
        return;
    }

    // 执行投掷
    throwShotPut(attacker);
}

// 修改投掷实心球方法
void ShotPut::throwShotPut(Character *attacker) {
    remainingThrows--;

    qDebug() << "Throwing shot put! Remaining throws:" << remainingThrows;

    // 获取战斗场景
    BattleScene *battleScene = qobject_cast<BattleScene*>(scene());
    if (!battleScene) {
        qDebug() << "Warning: Cannot get battle scene for projectile";
        return;
    }

    // 创建投掷物
    QPointF startPos = attacker->pos() + QPointF(attacker->isFacingRight() ? 50 : -50, -30);
    ShotPutProjectile *projectile = new ShotPutProjectile(
        startPos, attacker->isFacingRight(), attacker, battleScene, this
        );

    // 注册投掷物
    registerProjectile(projectile);

    // 添加到场景和BattleScene的投掷物管理
    battleScene->addItem(projectile);
    battleScene->addProjectile(projectile);

    if (!canThrow()) {
        qDebug() << "ShotPut depleted, scheduling weapon removal.";

        // 捕获必要的信息而不是指针
        // 注意：这里我们不能使用 QPointer，因为 Character 不是 QObject
        // 相反，我们让 BattleScene 来处理武器替换
        QTimer::singleShot(100, this, [battleScene, attacker]() {
            // 再次检查场景和角色是否仍然有效
            // 这是通过检查角色是否仍在场景中来实现的
            if (battleScene && battleScene->items().contains(attacker)) {
                if (!attacker->isDead()) {
                    qDebug() << "Executing delayed weapon removal for attacker.";
                    Fist* defaultWeapon = new Fist();
                    attacker->setWeapon(defaultWeapon);
                }
            }
        });
    }
}

// 获取剩余投掷次数
int ShotPut::getRemainingThrows() const {
    return remainingThrows;
}

// 检查是否还能投掷
bool ShotPut::canThrow() const {
    return remainingThrows > 0;
}

// 投掷物实现
ShotPutProjectile::ShotPutProjectile(QPointF startPos, bool facingRight,
                                     Character *thrower, BattleScene *scene,
                                     ShotPut *weapon,
                                     QGraphicsItem *parent)
    : QGraphicsEllipseItem(0, 0, 15, 15, parent),
    thrower(thrower),
    battleScene(scene),
    sourceWeapon(weapon),
    markedForDeletion(false) {

    // 设置起始位置
    setPos(startPos);

    // 设置外观
    setBrush(QBrush(Qt::magenta));
    setPen(QPen(Qt::black, 2));
    setZValue(50); // 确保在其他物体上方

    // 计算初始速度
    qreal angleRad = qDegreesToRadians(THROW_ANGLE);
    qreal vx = INITIAL_SPEED * qCos(angleRad) * (facingRight ? 1 : -1);
    qreal vy = -INITIAL_SPEED * qSin(angleRad); // 负值表示向上

    velocity = QPointF(vx, vy);
    qDebug() << "ShotPut projectile created at" << startPos
             << "with velocity" << velocity;
}

// 新增：检查是否标记为删除的方法
bool ShotPutProjectile::isMarkedForDeletion() const {
    return markedForDeletion;
}

// 更新投掷物位置
void ShotPutProjectile::updatePosition() {
    if (markedForDeletion) return;

    // 应用重力
    velocity.setY(velocity.y() + GRAVITY);

    // 更新位置
    QPointF newPos = pos() + velocity;
    setPos(newPos);

    // 检查是否落地或超出边界
    if (newPos.y() > battleScene->sceneRect().height() - 120 ||
        newPos.x() < 0 || newPos.x() > battleScene->sceneRect().width()) {

        qDebug() << "ShotPut projectile hit ground or boundary";
        markedForDeletion = true; // 标记删除而非立即删除
    }
}

// 修改 checkCollision 方法
void ShotPutProjectile::checkCollision() {
    if (markedForDeletion) return;

    // 检查投掷者和场景是否仍然有效
    if (!thrower || !battleScene || !battleScene->items().contains(thrower)) {
        qDebug() << "Thrower is invalid, marking projectile for deletion";
        markedForDeletion = true;
        return;
    }

    // 获取碰撞的物品
    QList<QGraphicsItem*> collidingItems = this->collidingItems();

    for (QGraphicsItem *item : collidingItems) {
        Character *character = qgraphicsitem_cast<Character*>(item);

        if (!character) continue;

        // 添加额外的有效性检查
        if (character == thrower || character->isDead()) {
            continue;
        }

        qDebug() << "ShotPut hit target! Dealing" << ShotPut::DAMAGE << "damage";

        // 检查sourceWeapon是否仍然有效
        if (sourceWeapon) {
            character->takeDamage(ShotPut::DAMAGE, sourceWeapon);
        } else {
            // 如果武器已被销毁，仍然造成伤害但不传递武器引用
            character->takeDamage(ShotPut::DAMAGE, nullptr);
        }

        markedForDeletion = true;
        return;
    }
}

ShotPutProjectile::~ShotPutProjectile() {
    qDebug() << "ShotPutProjectile destroyed";

    // 从武器的活跃列表中注销
    if (sourceWeapon) {
        sourceWeapon->unregisterProjectile(this);
    }
}

// 实现基类的纯虚函数
WeaponType ShotPut::getWeaponType() const {
    return WeaponType::ShotPut;
}
