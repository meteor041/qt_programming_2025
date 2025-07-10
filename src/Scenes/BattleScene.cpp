// src/Scenes/BattleScene.cpp

#include <QDebug>
#include <QLineF> // 用于计算距离
#include "BattleScene.h"
#include "../Items/Characters/Link.h" // 假设你的角色是Link
#include "../Items/Maps/Battlefield.h"
#include "../Items/Armors/FlamebreakerArmor.h"
#include "../Items/Maps/platform/Platform.h" // 包含平台基类

// 构造函数：初始化整个战斗场景
BattleScene::BattleScene(QObject *parent) : Scene(parent) {
    setSceneRect(0, 0, 1280, 720);

    // 【核心改动 #1】修复Battlefield的创建
    // Battlefield的构造函数现在需要一个指向场景的指针 (this)
    map = new Battlefield(this);
    addItem(map);
    map->setZValue(-1.0);
    map->scaleToFitScene(this); // 缩放背景图

    // 创建角色
    character = new Link();
    addItem(character);
    character->setScale(1.0);
    character->setZValue(10);
    // 使用新的getSpawnPos，它会基于平台位置来决定出生点
    QPointF spawnPos = map->getSpawnPos();
    character->setPos(spawnPos);

    enemy = new Link();
    addItem(enemy);
    enemy->setScale(1.0);
    enemy->setZValue(10);
    enemy->setPos(spawnPos.x() - 200, spawnPos.y());
    // 创建备用盔甲
    spareArmor = new FlamebreakerArmor();
    addItem(spareArmor);
    spareArmor->unmount();

    // 【核心改动 #2】修复盔甲的放置位置
    // 我们不能再使用 getFloorHeight()，而是要找到一个平台来放置它
    QPointF armorTargetPos(sceneRect().center().x() + 200, 0); // 设定一个目标水平位置
    Platform* groundForArmor = map->getGroundPlatform(armorTargetPos, spareArmor->boundingRect().height());
    if (groundForArmor) {
        // 如果找到了平台，将盔甲放在平台表面上方
        spareArmor->setPos(armorTargetPos.x(),
                           groundForArmor->getSurfaceY() - spareArmor->boundingRect().height());
    } else {
        // 如果没找到平台（作为备用方案），放在一个安全的位置
        spareArmor->setPos(sceneRect().center());
    }
}

// 【核心改动 #3】实现物理处理逻辑 (修正版)
void BattleScene::processPhysics() {
    if (!character || !map) {
        return;
    }

    // 定义物理常量
    const qreal GRAVITY = 0.8;
    const qreal MAX_FALL_SPEED = 20.0;

    //定义误差量，由于图片更新大小导致
    const qreal GROUND_TOLERANCE = 2.0;
    // --- 步骤 1: 应用重力 ---
    QPointF currentVelocity = character->getVelocity();
    currentVelocity.setY(currentVelocity.y() + GRAVITY);


    if (currentVelocity.y() > MAX_FALL_SPEED) {
        currentVelocity.setY(MAX_FALL_SPEED);
    }
    // 注意：我们暂时不把速度设置回去，因为如果发生碰撞，速度会被重置。

    // --- 步骤 2: 碰撞检测与解决 ---
    bool onGroundThisFrame = false;

    QRectF charRect = character->boundingRect();
    // 查找角色当前位置下方的最高平台
    Platform* ground = map->getGroundPlatform(character->pos(), charRect.height());

    if (ground != nullptr) {
        qreal surfaceY = ground->getSurfaceY();

        // 获取角色脚部在【当前帧开始时】的位置
        qreal footY_current = character->pos().y() + charRect.height();

        // 预测角色脚部在【当前帧结束时】的位置 (如果没有任何碰撞)
        qreal footY_next = footY_current + currentVelocity.y();

        // 【！！！核心修复！！！】
        // 新的碰撞条件：
        // 1. 角色正在下落 (velocity >= 0)。
        // 2. 在本帧开始时，脚还在平台上方。
        // 3. 在本帧移动后，脚将会移动到平台下方。
        // 这段逻辑能够捕捉到“穿越”平台表面的瞬间，无论速度有多快！
        if (currentVelocity.y() >= 0 && footY_current <= surfaceY && footY_next >= surfaceY) {

            // --- 碰撞解决！---
            // 发生了碰撞，将角色精确地放在平台表面
            character->setY(surfaceY - charRect.height());

            // 垂直速度清零
            currentVelocity.setY(0);

            // 标记角色已在地面上
            onGroundThisFrame = true;
        }
    }

    // --- 步骤 3: 更新最终的速度和状态 ---
    character->setVelocity(currentVelocity);
    character->setOnGround(onGroundThisFrame);
}


// --- 以下是其他函数的代码，大部分保持不变 ---

// 这个函数现在只负责应用最终计算出的速度来移动角色
void BattleScene::processMovement() {
    if (character != nullptr) {
        // deltaTime/1000.0 将毫秒转换为秒
        // 在我们的帧同步模型里，可以简化为直接应用速度
        QPointF newPos = character->pos() + character->getVelocity();

        // 获取边界
        QRectF bounds = sceneRect();
        QRectF charRect = character->boundingRect();

        // 应用边界限制
        newPos.setX(qBound(20.0, newPos.x(), bounds.width() - charRect.width()));
        // (Y轴边界可以暂时放宽，因为平台会阻止下落)
        newPos.setY(qBound(0.0, newPos.y(), bounds.height()));

        // 平台碰撞检测：检查角色是否会穿过平台
        if (map) {
            // 直接遍历所有平台进行碰撞检测
            for (Platform* platform : map->getPlatforms()) {
                QRectF platformRect = platform->sceneBoundingRect();
                
                // 计算角色当前和新位置的边界框
                QRectF currentCharRect(character->pos(), charRect.size());
                QRectF newCharRect(newPos, charRect.size());
                
                // 检查水平范围是否重叠
                bool horizontalOverlap = (newCharRect.right() > platformRect.left() && 
                                         newCharRect.left() < platformRect.right());
                
                // 检查垂直范围是否重叠
                bool verticalOverlap = (newCharRect.bottom() > platformRect.top() && 
                                       newCharRect.top() < platformRect.bottom());
                
                if (horizontalOverlap) {
                    // 1. 底部碰撞检测（角色下落撞到平台顶部）
                    if (character->getVelocity().y() > 0) { // 角色正在下落
                        qreal currentBottom = currentCharRect.bottom();
                        qreal newBottom = newCharRect.bottom();
                        qreal platformTop = platformRect.top();
                        
                        // 检查是否从平台上方穿过平台顶部
                        if (currentBottom <= platformTop && newBottom > platformTop) {
                            // 将角色位置限制在平台表面上方
                            newPos.setY(platformTop - charRect.height());
                            // 停止垂直速度（着陆）
                            character->setVelocity(QPointF(character->getVelocity().x(), 0));
                            break; // 找到碰撞就停止检查其他平台
                        }
                    }
                    
                    // 2. 头部碰撞检测（角色向上跳跃撞到平台底部）
                    else if (character->getVelocity().y() < 0) { // 角色正在上升
                        qreal currentTop = currentCharRect.top();
                        qreal newTop = newCharRect.top();
                        qreal platformBottom = platformRect.bottom();
                        
                        // 检查是否从平台下方撞到平台底部
                        if (currentTop >= platformBottom && newTop < platformBottom) {
                            // 将角色位置限制在平台底部下方
                            newPos.setY(platformBottom);
                            // 反转Y轴速度（向下弹回）
                            character->setVelocity(QPointF(character->getVelocity().x(), -character->getVelocity().y()));
                            break; // 找到碰撞就停止检查其他平台
                        }
                    }
                }
                // qDebug() << "垂直重叠" << currentCharRect << newCharRect << platformRect;
                
                // 3. 左右侧碰撞检测（新增）
                if (verticalOverlap) {
                    // 左侧碰撞检测（角色从左侧撞到平台）
                    if (character->getVelocity().x() > 0) { // 角色正在向右移动
                        qreal currentRight = currentCharRect.right();
                        qreal newRight = newCharRect.right();
                        qreal platformLeft = platformRect.left();
                        
                        // 检查是否从平台左侧穿过平台左边界
                        if (currentRight <= platformLeft && newRight > platformLeft) {
                            // 将角色位置限制在平台左侧
                            newPos.setX(platformLeft - charRect.width());
                            // 反转X轴速度
                            character->setVelocity(QPointF(-character->getVelocity().x(), character->getVelocity().y()));
                            break; // 找到碰撞就停止检查其他平台
                        }
                    }
                    
                    // 右侧碰撞检测（角色从右侧撞到平台）
                    else if (character->getVelocity().x() < 0) { // 角色正在向左移动
                        qreal currentLeft = currentCharRect.left();
                        qreal newLeft = newCharRect.left();
                        qreal platformRight = platformRect.right();
                        
                        // 检查是否从平台右侧穿过平台右边界
                        if (currentLeft >= platformRight && newLeft < platformRight) {
                            // 将角色位置限制在平台右侧
                            newPos.setX(platformRight);
                            // 反转X轴速度
                            character->setVelocity(QPointF(-character->getVelocity().x(), character->getVelocity().y()));
                            break; // 找到碰撞就停止检查其他平台
                        }
                    }
                }
            }
        }
        
        // 修改Y轴限制：不允许进入画面最下方120像素的区域
        newPos.setY(qBound(0.0, newPos.y(), bounds.height() - charRect.height() - 120.0));
        character->setPos(newPos);
    }
}


void BattleScene::processInput() {
    Scene::processInput();
    if (character != nullptr) {
        character->processInput(); // 假设角色内部会根据按键状态设置速度
    }
}

void BattleScene::keyPressEvent(QKeyEvent *event) {
    // 阻止重复触发
    if (event->isAutoRepeat()) {
        return;
    }

    switch (event->key()) {
    case Qt::Key_A:
        if (character) character->setLeftDown(true);
        break;
    case Qt::Key_D:
        if (character) character->setRightDown(true);
        break;

    // --- 【核心改动 C1】修改拾取键，并添加下蹲键 ---
    case Qt::Key_S: // 使用S键作为下蹲和拾取的复合键
        if (character) {
            character->setPickDown(true);   // 意图：拾取
            character->setCrouchDown(true); // 意图：下蹲
        }
        break;

    case Qt::Key_W:
    case Qt::Key_Space:
        if (character && character->isOnGround()) {
            // 【修改】下蹲时不能跳跃
            if (!character->isCrouching()) {
                character->setJumpDown(true);
            }
        }
        break;
    // 新增：K键攻击
    case Qt::Key_K:
        if (character && !character->isDead()) {
            attackKeyDown = true;
        }
        break;
    default:
        Scene::keyPressEvent(event);
    }
}

void BattleScene::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) {
        return;
    }

    switch (event->key()) {
    case Qt::Key_A:
        if (character) character->setLeftDown(false);
        break;
    case Qt::Key_D:
        if (character) character->setRightDown(false);
        break;

    // --- 【核心改动 C2】同步修改松开事件 ---
    case Qt::Key_S: // 松开S键
        if (character) {
            character->setPickDown(false);
            character->setCrouchDown(false);
        }
        break;

    case Qt::Key_W:
    case Qt::Key_Space:
        if (character) character->setJumpDown(false);
        break;
    // 新增：K键释放
    case Qt::Key_K:
        attackKeyDown = false;
        break;
    default:
        Scene::keyReleaseEvent(event);
    }
}

// 新增：战斗处理函数
void BattleScene::processCombat() {
    if (!character || !enemy || character->isDead()) {
        return;
    }
    
    // 处理角色攻击
    if (attackKeyDown) {
        // 检查敌人是否在攻击范围内
        if (isInAttackRange(character, enemy, character->getWeaponAttackRange())) {
            // 执行攻击
            character->performAttack();
            
            // 对敌人造成伤害（这里设定基础伤害为20）
            int damage = 20;
            enemy->takeDamage(damage);
            
            qDebug() << "Character attacked enemy! Enemy health:" << enemy->getHealth();
        } else {
            qDebug() << "Enemy is too far to attack!";
        }
        
        // 重置攻击状态，避免连续攻击
        attackKeyDown = false;
    }
}

// 新增：检查攻击范围的辅助函数
bool BattleScene::isInAttackRange(Character* attacker, Character* target, qreal range) {
    if (!attacker || !target) {
        return false;
    }
    
    QPointF attackerPos = attacker->pos();
    QPointF targetPos = target->pos();
    
    // 计算两个角色之间的距离
    qreal distance = QLineF(attackerPos, targetPos).length();
    
    return distance <= range;
}


void BattleScene::update() {
    // 添加战斗处理
    processCombat();
    // BattleScene的update只需要调用基类的update即可，所有逻辑都在各个process函数里
    Scene::update();
}

void BattleScene::processPicking() {
    Scene::processPicking();
    if (character && character->isPicking()) {
        auto mountable = findNearestUnmountedMountable(character->pos(), 100.);
        if (mountable != nullptr) {
            spareArmor = dynamic_cast<Armor *>(pickupMountable(character, mountable));
        }
    }
}

Mountable *BattleScene::findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold) {
    Mountable *nearest = nullptr;
    qreal minDistance = distance_threshold;
    for (QGraphicsItem *item: items()) {
        if (auto mountable = dynamic_cast<Mountable *>(item)) {
            if (!mountable->isMounted()) {
                qreal distance = QLineF(pos, item->pos()).length();
                if (distance < minDistance) {
                    minDistance = distance;
                    nearest = mountable;
                }
            }
        }
    }
    return nearest;
}

Mountable *BattleScene::pickupMountable(Character *character, Mountable *mountable) {
    if (auto armor = dynamic_cast<Armor *>(mountable)) {
        return character->pickupArmor(armor);
    }
    return nullptr;
}
