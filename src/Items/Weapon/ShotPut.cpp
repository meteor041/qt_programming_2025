#include "ShotPut.h"
#include "../Characters/Character.h"
#include "../../Scenes/BattleScene.h"
#include <QDebug>
#include <QPixmap>
#include <QGraphicsScene>
#include <QtMath>


// 实心球武器构造函数
ShotPut::ShotPut(QGraphicsItem *parent, int throwCount) 
    : Weapon(parent, QString::number(ShotPut::DAMAGE)), maxThrows(throwCount), remainingThrows(throwCount) {
    
    // 设置攻击范围（投掷距离）
    attackRange = 300.0;
    
    // 设置实心球图片
    if (!pixmapItem) {
        pixmapItem = new QGraphicsPixmapItem(this);
    }
    if (pixmapItem) {
        pixmapItem->setPixmap(QPixmap(":/shotput.png"));
        pixmapItem->setPos(30, -10);
    }
    
    qDebug() << "ShotPut created with" << maxThrows << "throws";
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
        // 自动卸载武器
        // attacker->setWeapon(new Fist());
        return;
    }
    
    // 执行投掷
    throwShotPut(attacker);
}

// 投掷实心球
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
    // 创建投掷物时传入this指针
    ShotPutProjectile *projectile = new ShotPutProjectile(
        startPos, attacker->isFacingRight(), attacker, battleScene, this // 传入this
    );
    
    // 添加到场景和BattleScene的投掷物管理
    battleScene->addItem(projectile);
    battleScene->addProjectile(projectile);
    
    // 如果用完所有投掷次数，移除武器
    // ShotPut.cpp, line ~52
    if (!canThrow()) {
        qDebug() << "ShotPut depleted, removing weapon";

        // 1. 切换到新武器
        attacker->setWeapon(new Fist(attacker)); // 建议给Fist也传递parent

        // 2. 从场景中移除自己（如果自己的图标在场景中）
        if (this->scene()) {
            this->scene()->removeItem(this);
        }

        // 3. 删除自己
        delete this;

        // 4. 【重要】在调用 delete this 之后立即返回，不要再执行任何代码
        return;
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
                                   ShotPut *weapon, // 新增参数
                                   QGraphicsItem *parent)
    : QGraphicsEllipseItem(0, 0, 15, 15, parent), 
      thrower(thrower), 
      battleScene(scene), 
      sourceWeapon(weapon), // 初始化新成员
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
    
    initialVelocity = QPointF(vx, vy);
    velocity = initialVelocity;
    
    // 投掷物现在由BattleScene统一管理，不再使用QTimer
    
    qDebug() << "ShotPut projectile created at" << startPos 
             << "with velocity" << velocity;
}

// onUpdateTimer方法已移除，现在由BattleScene统一调用updatePosition和checkCollision


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

// 修复第185行的takeDamage调用
void ShotPutProjectile::checkCollision() {
    if (markedForDeletion) return;
    // 获取碰撞的物品
    QList<QGraphicsItem*> collidingItems = this->collidingItems();
    qDebug() << "Colliding items count:" << collidingItems.size();
    
    for (QGraphicsItem *item : collidingItems) {
        qDebug() << "Colliding with item type:" << typeid(*item).name();
        Character *character = qgraphicsitem_cast<Character*>(item);
        
        if (character) {
            qDebug() << "Found character. Is thrower?" << (character == thrower) 
                     << "Is dead?" << character->isDead();
            
            // 添加额外的有效性检查
            if (!character || character == thrower || character->isDead()) {
                qDebug() << "Skipping invalid or dead character";
                continue;
            }
        }
        
        // 如果撞到角色且不是投掷者且角色活着
        if (character && character != thrower && !character->isDead()) {
            qDebug() << "ShotPut hit target! Dealing" << ShotPut::DAMAGE << "damage";
            
            // 造成伤害 - 修复：传入sourceWeapon作为第二个参数
            character->takeDamage(ShotPut::DAMAGE, sourceWeapon);
            
            // 直接删除对象
            markedForDeletion = true; // 标记删除
            return;
        }
    }
}

ShotPutProjectile::~ShotPutProjectile() {
    qDebug() << "ShotPutProjectile destroyed";
}


// 实现基类的纯虚函数
WeaponType ShotPut::getWeaponType() const {
    return WeaponType::ShotPut;
}
