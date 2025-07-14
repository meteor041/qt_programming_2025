// src/Scenes/BattleScene.cpp

#include <QDebug>
#include <QLineF> // 用于计算距离
#include "BattleScene.h"
#include "../Items/Characters/Link.h" // 假设你的角色是Link
#include "../Items/Maps/Battlefield.h"
#include "../Items/Armors/ChainmailArmor.h"
#include "../Items/Maps/platform/Platform.h" // 包含平台基类
#include "../Items/Weapon/ShotPut.h" // 包含投掷物类
#include "../Items/Weapon/ShotPutProjectile.h"
#include "../Items/Weapon/Bullet.h"

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
    // 将玩家2放置在场景右侧，与玩家1对称
    enemy->setPos(sceneRect().width() - spawnPos.x() - enemy->boundingRect().width(), spawnPos.y());
    
    // 初始化血条UI
    initHealthBars();
    //初始化护甲UI
    initArmorDisplays();

    // 【新增】初始化FPS显示
    fpsTextItem = new QGraphicsTextItem();
    fpsTextItem->setFont(QFont("Arial", 14, QFont::Bold));
    fpsTextItem->setDefaultTextColor(Qt::yellow); // 使用醒目的颜色
    // 将其放置在屏幕顶部中央
    fpsTextItem->setPos(sceneRect().width() / 2 - 50, 10);
    fpsTextItem->setZValue(103); // 确保在血条之上
    addItem(fpsTextItem);

    // 【新增】初始化游戏结束文本
    m_gameOverText = new QGraphicsTextItem();
    m_gameOverText->setFont(QFont("Arial", 48, QFont::Bold));
    m_gameOverText->setDefaultTextColor(Qt::red);
    m_gameOverText->setZValue(105); // 确保在所有UI之上
    m_gameOverText->setVisible(false); // 初始时隐藏
    addItem(m_gameOverText);


    // 启动FPS计时器
    fpsTimer.start();
}

// 【核心改动 #3】实现物理处理逻辑 (修正版)
// 【推荐】创建一个辅助函数（未在头文件声明，仅用于本文件）来处理单个角色的物理和平台事件
//  避免对于不同角色代码重复
static void processCharacterPhysicsAndEvents(Character* character, Map* map) {
    if (!character || !map) {
        return;
    }

    // --- 物理计算部分 (保持不变) ---
    const qreal GRAVITY = 0.8;
    const qreal MAX_FALL_SPEED = 20.0;
    QPointF currentVelocity = character->getVelocity();
    currentVelocity.setY(currentVelocity.y() + GRAVITY);
    if (currentVelocity.y() > MAX_FALL_SPEED) {
        currentVelocity.setY(MAX_FALL_SPEED);
    }

    // --- 碰撞检测部分 (保持不变) ---
    bool onGroundThisFrame = false;
    QRectF charRect = character->boundingRect();
    Platform* groundThisFrame = map->getGroundPlatform(character->pos(), charRect.height());

    if (groundThisFrame != nullptr) {
        qreal surfaceY = groundThisFrame->getSurfaceY();
        qreal footY_current = character->pos().y() + charRect.height();
        qreal footY_next = footY_current + currentVelocity.y();

        if (currentVelocity.y() >= 0 && footY_current <= surfaceY && footY_next >= surfaceY) {
            character->setY(surfaceY - charRect.height());
            currentVelocity.setY(0);
            onGroundThisFrame = true;
        }
    }

    // --- 【核心改动】平台事件驱动逻辑 ---
    Platform* lastFramePlatform = character->getCurrentPlatform();

    // 1. 检查平台是否发生变化
    if (groundThisFrame != lastFramePlatform) {
        // 触发 "离开" 事件
        if (lastFramePlatform) {
            lastFramePlatform->onCharacterLeave(character);
        }
        // 触发 "进入" 事件
        if (groundThisFrame) {
            groundThisFrame->onCharacterEnter(character);
        }
    }

    // 2. 如果角色在某个平台上，则每帧触发 "停留" 事件
    if (groundThisFrame) {
        groundThisFrame->onCharacterStay(character);
    }

    // 3. 更新角色当前所在的平台记录
    character->setCurrentPlatform(groundThisFrame);


    // --- 更新最终速度和状态 (保持不变) ---
    character->setVelocity(currentVelocity);
    character->setOnGround(onGroundThisFrame);
}


// 【修改】processPhysics 函数，使其调用新的辅助函数
void BattleScene::processPhysics() {
    // 为玩家角色处理
    processCharacterPhysicsAndEvents(character, map);

    // 为敌人角色处理
    processCharacterPhysicsAndEvents(enemy, map);
}


// --- 以下是其他函数的代码，大部分保持不变 ---

// 这个函数现在只负责应用最终计算出的速度来移动角色
// 【新增】用于处理单个角色移动的辅助函数，避免代码重复
// 这个函数现在只负责应用最终计算出的速度来移动角色
void BattleScene::processCharacterMovement(Character* aCharacter) {
    // 检查传入的指针是否有效
    if (aCharacter == nullptr) {
        return;
    }

    // 在我们的帧同步模型里，可以简化为直接应用速度
    QPointF newPos = aCharacter->pos() + aCharacter->getVelocity();

    // 获取边界
    QRectF bounds = sceneRect();
    QRectF charRect = aCharacter->boundingRect();

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
            QRectF currentCharRect(aCharacter->pos(), charRect.size());
            QRectF newCharRect(newPos, charRect.size());

            // 检查水平范围是否重叠
            bool horizontalOverlap = (newCharRect.right() > platformRect.left() &&
                                      newCharRect.left() < platformRect.right());

            // 检查垂直范围是否重叠
            bool verticalOverlap = newCharRect.intersects(platformRect);

            if (horizontalOverlap) {
                // 1. 底部碰撞检测（角色下落撞到平台顶部）
                if (aCharacter->getVelocity().y() > 0) { // 角色正在下落
                    qreal currentBottom = currentCharRect.bottom();
                    qreal newBottom = newCharRect.bottom();
                    qreal platformTop = platformRect.top();

                    // 检查是否从平台上方穿过平台顶部
                    if (currentBottom <= platformTop && newBottom > platformTop) {
                        // 将角色位置限制在平台表面上方
                        newPos.setY(platformTop - charRect.height());
                        // 停止垂直速度（着陆）
                        aCharacter->setVelocity(QPointF(aCharacter->getVelocity().x(), 0));
                        break; // 找到碰撞就停止检查其他平台
                    }
                }

                // 2. 头部碰撞检测（角色向上跳跃撞到平台底部）
                else if (aCharacter->getVelocity().y() < 0) { // 角色正在上升
                    qreal currentTop = currentCharRect.top();
                    qreal newTop = newCharRect.top();
                    qreal platformBottom = platformRect.bottom();

                    // 检查是否从平台下方撞到平台底部
                    if (currentTop >= platformBottom && newTop < platformBottom) {
                        // 将角色位置限制在平台底部下方
                        newPos.setY(platformBottom);
                        // 反转Y轴速度（向下弹回）
                        aCharacter->setVelocity(QPointF(aCharacter->getVelocity().x(), -aCharacter->getVelocity().y()));
                        break; // 找到碰撞就停止检查其他平台
                    }
                }
            }
            // qDebug() << "垂直重叠" << currentCharRect << newCharRect << platformRect;

            // 3. 左右侧碰撞检测（新增）
            if (verticalOverlap) {
                 qDebug() << "vertical overlap"; // 如果需要调试，可以取消此行注释
                // 左侧碰撞检测（角色从左侧撞到平台）
                if (aCharacter->getVelocity().x() > 0) { // 角色正在向右移动
                    qreal currentRight = currentCharRect.right();
                    qreal newRight = newCharRect.right();
                    qreal platformLeft = platformRect.left();

                    // 检查是否从平台左侧穿过平台左边界
                    if (currentRight <= platformLeft && newRight > platformLeft) {
                        // 将角色位置限制在平台左侧
                        newPos.setX(platformLeft - charRect.width());
                        // 反转X轴速度
                        aCharacter->setVelocity(QPointF(-aCharacter->getVelocity().x(), aCharacter->getVelocity().y()));
                        break; // 找到碰撞就停止检查其他平台
                    }
                }

                // 右侧碰撞检测（角色从右侧撞到平台）
                else if (aCharacter->getVelocity().x() < 0) { // 角色正在向左移动
                    qreal currentLeft = currentCharRect.left();
                    qreal newLeft = newCharRect.left();
                    qreal platformRight = platformRect.right();

                    // 检查是否从平台右侧穿过平台右边界
                    if (currentLeft >= platformRight && newLeft < platformRight) {
                        qDebug() << "vertical conflict";
                        // 将角色位置限制在平台右侧
                        newPos.setX(platformRight);
                        qDebug() << "set x";
                        // 反转X轴速度
                        aCharacter->setVelocity(QPointF(-aCharacter->getVelocity().x(), aCharacter->getVelocity().y()));
                        break; // 找到碰撞就停止检查其他平台
                    }
                }
            }
        }
    }
//？为什么会出现跳跃时穿过的情况
    // 修改Y轴限制：不允许进入画面最下方120像素的区域
    newPos.setY(qBound(0.0, newPos.y(), bounds.height() - charRect.height() - 120.0));
    aCharacter->setPos(newPos);
}

// 【核心改动 #2】重构 processMovement 以调用通用函数
void BattleScene::processMovement() {
    processCharacterMovement(character); // 为玩家1处理移动
    processCharacterMovement(enemy);     // 为玩家2处理移动
}


// processInput现在也需要为enemy调用
void BattleScene::processInput() {
    Scene::processInput();
    if (character != nullptr) {
        character->processInput();
    }
    if (enemy != nullptr) {
        enemy->processInput();
    }
}

// 【核心改动 #3】更新键盘按下事件以包含玩家2的控制
void BattleScene::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) {
        return;
    }

    bool keyHandled = true; // 假设我们能处理这个按键

    // --- 玩家1 (WASD/K) 控制 ---
    switch (event->key()) {
    case Qt::Key_A:
        if (character) character->setLeftDown(true);
        break;
    case Qt::Key_D:
        if (character) character->setRightDown(true);
        break;
    case Qt::Key_S:
        if (character) {
            character->setPickDown(true);
            character->setCrouchDown(true);
        }
        break;
    case Qt::Key_W:
    case Qt::Key_Space:
        if (character && character->isOnGround() && !character->isCrouching()) {
            character->setJumpDown(true);
        }
        break;
    case Qt::Key_K:
        if (character && !character->isDead()) {
            attackKeyDown = true;
        }
        break;
    }

    // --- 玩家2 (方向键/0) 控制 ---
    switch (event->key()) {
    case Qt::Key_Left:
        if (enemy) enemy->setLeftDown(true);
        break;
    case Qt::Key_Right:
        if (enemy) enemy->setRightDown(true);
        break;
    case Qt::Key_Down:
        if (enemy) {
            enemy->setPickDown(true);
            enemy->setCrouchDown(true);
        }
        break;
    case Qt::Key_Up:
        if (enemy && enemy->isOnGround() && !enemy->isCrouching()) {
            enemy->setJumpDown(true);
        }
        break;
    case Qt::Key_0: // 数字键0
        if (enemy && !enemy->isDead()) {
            enemyAttackKeyDown = true;
        }
        break;
        // --- 其他按键 ---
    default:
        keyHandled = false; // 我们不处理这个按键
        break;
    }

    if (keyHandled) {
        event->accept(); // 【重要】接受事件，阻止它传播到 QGraphicsView
    } else {
        Scene::keyPressEvent(event); // 如果不是我们的控制键，就交给基类处理
    }
}
// 【核心改动 #4】更新键盘释放事件以包含玩家2的控制
void BattleScene::keyReleaseEvent(QKeyEvent *event) {
    if (event->isAutoRepeat()) {
        return;
    }

    bool keyHandled = true; // 假设我们能处理这个按键

    // --- 玩家1 (WASD/K) 控制 ---
    switch (event->key()) {
    case Qt::Key_A:
        if (character) character->setLeftDown(false);
        break;
    case Qt::Key_D:
        if (character) character->setRightDown(false);
        break;
    case Qt::Key_S:
        if (character) {
            character->setPickDown(false);
            character->setCrouchDown(false);
        }
        break;
    case Qt::Key_W:
    case Qt::Key_Space:
        if (character) character->setJumpDown(false);
        break;
    case Qt::Key_K:
        attackKeyDown = false;
        break;
    }

    // --- 玩家2 (方向键/0) 控制 ---
    switch (event->key()) {
    case Qt::Key_Left:
        if (enemy) enemy->setLeftDown(false);
        break;
    case Qt::Key_Right:
        if (enemy) enemy->setRightDown(false);
        break;
    case Qt::Key_Down:
        if (enemy) {
            enemy->setPickDown(false);
            enemy->setCrouchDown(false);
        }
        break;
    case Qt::Key_Up:
        if (enemy) enemy->setJumpDown(false);
        break;
    case Qt::Key_0:
        enemyAttackKeyDown = false;
        break;
        // --- 其他按键 ---
    default:
        keyHandled = false; // 我们不处理这个按键
        break;
    }

    if (keyHandled) {
        event->accept(); // 【重要】接受事件
    } else {
        Scene::keyReleaseEvent(event); // 交给基类处理
    }
}

// 【新增】处理单个角色攻击的辅助函数
void BattleScene::processCharacterCombat(Character* attacker, Character* target, bool& attackFlag) {
    if (!attacker || !target || !attackFlag || attacker->isDead()) {
        return;
    }

    Weapon* weapon = attacker->getWeapon();
    if (!weapon) {
        qDebug() << "No weapon equipped!";
        return;
    }

    // --- 修改后的逻辑 ---
    WeaponType type = weapon->getWeaponType();

    if (type == WeaponType::Fist || type == WeaponType::Knife) {
        // 这是近战武器，需要检查攻击范围
        if (isInAttackRange(attacker, target, weapon->getAttackRange())) {
            attacker->performAttack();
            int damage = weapon->getAttackPower();
            target->takeDamage(damage, weapon);
            qDebug() << "Melee attack hit for" << damage << "damage!";
        } else {
            // 在范围外，可以播一个挥空的动画，但不开火
            qDebug() << "Melee attack missed, target out of range.";
        }
    } else if (type == WeaponType::Rifle || type == WeaponType::SniperRifle || type == WeaponType::ShotPut) {
        // 这是远程/投掷武器，不需要检查范围，直接让武器自己处理攻击逻辑
        attacker->performAttack();
        // 伤害和子弹创建由武器的 attack() 方法内部处理
    }

    // 重置攻击状态，实现单次按下、单次攻击
    attackFlag = false;
}

// 【改动】重构战斗处理函数，使其对称并可复用
void BattleScene::processCombat() {
    // 处理玩家1对玩家2的攻击
    processCharacterCombat(character, enemy, attackKeyDown);

    // 处理玩家2对玩家1的攻击
    processCharacterCombat(enemy, character, enemyAttackKeyDown);
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
    
    // 检查距离是否在攻击范围内
    if (distance > range) {
        return false;
    }
    
    // 检查目标是否在攻击者面向的方向
    bool targetIsOnRight = targetPos.x() > attackerPos.x();
    bool attackerFacingRight = attacker->isFacingRight();
    
    // 只有当目标在攻击者面向的方向时才能攻击
    return targetIsOnRight == attackerFacingRight;
}

// 【新增】护甲掉落处理函数
void BattleScene::processArmorDrop() {
    armorDropFrameCounter++;

    if (armorDropFrameCounter >= ARMOR_DROP_INTERVAL) {
        armorDropFrameCounter = 0;

        Armor* newArmor = createRandomArmor();
        if (newArmor) {
            qreal randomX = QRandomGenerator::global()->bounded(static_cast<int>(50), static_cast<int>(sceneRect().width() - 50));
            newArmor->setPos(randomX, 0);
            addItem(newArmor);
            fallingArmors.append(newArmor);
            qDebug() << "Armor dropped at position:" << newArmor->pos();
        }
    }

    updateFallingArmors();
}

// 【新增】创建随机护甲
Armor* BattleScene::createRandomArmor() {
    int armorType = QRandomGenerator::global()->bounded(2); // 0 或 1

    Armor* armor = nullptr;
    switch (armorType) {
    case 0:
        armor = new ChainmailArmor();
        qDebug() << "Created Chainmail Armor";
        break;
    case 1:
        armor = new BulletproofVest();
        qDebug() << "Created Bulletproof Vest";
        break;
    default:
        armor = new ChainmailArmor();
        break;
    }

    return armor;
}

// 【新增】更新下落中的护甲
void BattleScene::updateFallingArmors() {
    auto it = fallingArmors.begin();
    while (it != fallingArmors.end()) {
        Armor* armor = *it;
        QPointF newPos = armor->pos() + QPointF(0, ARMOR_FALL_SPEED);

        bool hasLanded = false;
        if (map) {
            Platform* groundPlatform = map->getGroundPlatform(newPos, armor->boundingRect().height());
            if (groundPlatform) {
                qreal surfaceY = groundPlatform->getSurfaceY();
                if (newPos.y() + armor->boundingRect().height() >= surfaceY) {
                    newPos.setY(surfaceY - armor->boundingRect().height());
                    hasLanded = true;
                }
            }
        }

        if (newPos.y() + armor->boundingRect().height() >= sceneRect().height() - 120) {
            newPos.setY(sceneRect().height() - 120 - armor->boundingRect().height());
            hasLanded = true;
        }

        armor->setPos(newPos);

        if (hasLanded) {
            it = fallingArmors.erase(it);
        } else {
            ++it;
        }
    }
}


void BattleScene::update() {
    // 【核心修改】在所有逻辑开始前检查游戏是否结束
    if (m_isGameOver) {
        return; // 如果游戏已结束，则停止所有更新
    }

    // 【核心修改】检查胜利/失败条件
    if (character && character->isDead()) {
        showGameOverScreen("Enemy Wins!");
        return; // 立即返回，本帧不再处理其他逻辑
    }
    if (enemy && enemy->isDead()) {
        showGameOverScreen("Player 1 Wins!");
        return; // 立即返回，本帧不再处理其他逻辑
    }
    // 【新增】FPS计算和显示逻辑
    frameCount++;
    // 每隔大约1秒更新一次FPS文本
    if (fpsTimer.elapsed() > 1000) {
        // 计算FPS（帧数 * 1000 / 经过的毫秒数）
        qreal fps = (frameCount * 1000.0) / fpsTimer.elapsed();
        fpsTextItem->setPlainText(QString("FPS: %1").arg(qRound(fps)));

        // 重置计时器和帧计数器
        fpsTimer.restart();
        frameCount = 0;
    }

    // --- 原有的 update() 内容 ---
    // 添加战斗处理
    processCombat();
    // 添加武器掉落处理
    processWeaponDrop();
    // 添加消耗品掉落处理
    processConsumableDrop();
    // 【新增】添加护甲掉落处理
    processArmorDrop();
    // 【新增】在帧末尾调用清理函数
    processDeletions();
    // 【新增】更新护甲UI
    updateArmorDisplays();
    // 更新血条UI
    updateHealthBars();
    // BattleScene的update只需要调用基类的update即可，所有逻辑都在各个process函数里
    Scene::update();
}

// 新增：武器掉落处理函数
void BattleScene::processWeaponDrop() {
    // 增加帧计数器
    weaponDropFrameCounter++;
    
    // 检查是否到了掉落武器的时间（每900帧）
    if (weaponDropFrameCounter >= WEAPON_DROP_INTERVAL) {
        // 重置计数器
        weaponDropFrameCounter = 0;
        
        // 创建随机武器
        Weapon* newWeapon = createRandomWeapon();
        if (newWeapon) {
            // 设置武器的随机X位置（屏幕顶部）
            qreal randomX = QRandomGenerator::global()->bounded(static_cast<int>(50), static_cast<int>(sceneRect().width() - 50));
            newWeapon->setPos(randomX, 0);  // Y轴为0（屏幕顶部）
            
            // 添加到场景中
            addItem(newWeapon);
            
            // 添加到下落武器列表中
            fallingWeapons.append(newWeapon);
            
            qDebug() << "Weapon dropped at position:" << randomX << ", 0";
        }
    }
    
    // 更新所有正在下落的武器
    updateFallingWeapons();
}

// 在 BattleScene.h 的 private 部分添加声明:
// void processDeletions();

// 在 BattleScene.cpp 中实现:
void BattleScene::processDeletions() {
    // 创建一个列表来存储待删除的物品
    QList<QGraphicsItem*> itemsToDelete;

    // 遍历场景中的所有物品
    for (QGraphicsItem* item : items()) {
        // 检查是否是子弹
        if (auto* bullet = dynamic_cast<Bullet*>(item)) {
            if (bullet->isMarkedForDeletion()) {
                itemsToDelete.append(bullet);
            }
        }
        // 【新增】检查是否是实心球投掷物
        else if (auto* shotput = dynamic_cast<ShotPutProjectile*>(item)) {
            if (shotput->isMarkedForDeletion()) {
                itemsToDelete.append(shotput);
            }
        }
        // 【新增】检查是否是用完的实心球武器
        else if (auto* shotputWeapon = dynamic_cast<ShotPut*>(item)) {
            if (shotputWeapon->isMarkedForDeletion()) {
                itemsToDelete.append(shotputWeapon);
            }
        }
        // --- 【新增】检查损坏的护甲 ---
        else if (auto* armor = dynamic_cast<Armor*>(item)) {
            if (armor->isMarkedForDeletion()) {
                itemsToDelete.append(armor);
            }
        }
        // 未来还可以添加其他需要延迟删除的物品...
    }

    // 现在，安全地在另一个循环中删除所有被标记的物品
#include <QTimer> // 确保包含了这个头文件

    // ... 在你的函数中 ...
    for (QGraphicsItem* item : itemsToDelete) {
        qDebug() << "Marking item for cleanup:" << item;

        // 1. 立即从场景中移除，这样场景就不会再渲染或与它交互
        removeItem(item);

        // 2. 使用单次触发的定时器来安全地延迟删除
        //    这会将 `delete item;` 推迟到事件循环的下一个迭代中执行
        QTimer::singleShot(0, [item]() {
            qDebug() << "Actually deleting item:" << item;
            delete item;
        });
    }
    // 清空列表，因为它现在只包含悬空指针或即将被删除的对象的指针
    itemsToDelete.clear();
}

// 新增：创建随机武器
// 在 createRandomWeapon() 函数中修改
Weapon* BattleScene::createRandomWeapon() {
    // 之前你的随机数可能是 bounded(4)，现在需要根据武器总数调整
    int weaponType = QRandomGenerator::global()->bounded(4);  // 假设现在总共有4种武器: Knife, Rifle, SniperRifle, ShotPut

    Weapon* weapon = nullptr;
    switch (weaponType) {
    case 0:
        weapon = new Knife();
        qDebug() << "Created Knife weapon";
        break;
    case 1:
        weapon = new Rifle();
        qDebug() << "Created Rifle weapon";
        break;
    case 2:
        weapon = new SniperRifle();
        qDebug() << "Created SniperRifle weapon";
        break;
    case 3: // <-- 新增
        weapon = new ShotPut(nullptr, 3); // 默认3次使用机会
        qDebug() << "Created ShotPut weapon";
        break;
    default:
        weapon = new Knife();
        break;
    }

    return weapon;
}
// 新增：更新正在下落的武器
void BattleScene::updateFallingWeapons() {
    // 使用迭代器遍历，以便安全地删除元素
    auto it = fallingWeapons.begin();
    while (it != fallingWeapons.end()) {
        Weapon* weapon = *it;
        
        // 更新武器位置（向下移动）
        QPointF currentPos = weapon->pos();
        QPointF newPos = currentPos + QPointF(0, WEAPON_FALL_SPEED);
        
        // 检查武器是否落到地面上
        bool hasLanded = false;
        if (map) {
            Platform* groundPlatform = map->getGroundPlatform(newPos, weapon->boundingRect().height());
            if (groundPlatform) {
                qreal surfaceY = groundPlatform->getSurfaceY();
                qreal weaponBottom = newPos.y() + weapon->boundingRect().height();
                
                // 如果武器底部接触或穿过平台表面
                if (weaponBottom >= surfaceY) {
                    // 将武器放置在平台表面上
                    newPos.setY(surfaceY - weapon->boundingRect().height());
                    hasLanded = true;
                }
            }
        }
        
        // 检查武器是否落到场景底部
        if (newPos.y() + weapon->boundingRect().height() >= sceneRect().height() - 120) {
            newPos.setY(sceneRect().height() - 120 - weapon->boundingRect().height());
            hasLanded = true;
        }
        
        // 更新武器位置
        weapon->setPos(newPos);
        
        // 如果武器已经着陆，从下落列表中移除
        if (hasLanded) {
            qDebug() << "Weapon landed at position:" << newPos;
            it = fallingWeapons.erase(it);
        } else {
            ++it;
        }
    }
}

// 新增：初始化血条UI
void BattleScene::initHealthBars() {
    // 角色血条（左上角）
    qreal characterBarX = 20;
    qreal characterBarY = 20;

    // 角色血条背景
    characterHealthBarBg = new QGraphicsRectItem(characterBarX, characterBarY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT);
    characterHealthBarBg->setBrush(QBrush(Qt::darkRed));
    characterHealthBarBg->setPen(QPen(Qt::black, 2));
    characterHealthBarBg->setZValue(100);  // 确保在最前面
    addItem(characterHealthBarBg);

    // 角色血条前景
    characterHealthBarFg = new QGraphicsRectItem(characterBarX, characterBarY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT);
    characterHealthBarFg->setBrush(QBrush(Qt::green));
    characterHealthBarFg->setPen(QPen(Qt::transparent));
    characterHealthBarFg->setZValue(101);
    addItem(characterHealthBarFg);

    // 角色血量文字
    characterHealthText = new QGraphicsTextItem();
    characterHealthText->setPos(characterBarX, characterBarY + HEALTH_BAR_HEIGHT + 5);
    characterHealthText->setDefaultTextColor(Qt::white);
    characterHealthText->setFont(QFont("Arial", 12, QFont::Bold));
    characterHealthText->setZValue(102);
    addItem(characterHealthText);

    // 敌人血条（右上角）
    qreal enemyBarX = sceneRect().width() - HEALTH_BAR_WIDTH - 70;
    qreal enemyBarY = 20;

    // 敌人血条背景
    enemyHealthBarBg = new QGraphicsRectItem(enemyBarX, enemyBarY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT);
    enemyHealthBarBg->setBrush(QBrush(Qt::darkRed));
    enemyHealthBarBg->setPen(QPen(Qt::black, 2));
    enemyHealthBarBg->setZValue(100);
    addItem(enemyHealthBarBg);

    // 敌人血条前景
    enemyHealthBarFg = new QGraphicsRectItem(enemyBarX, enemyBarY, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT);
    enemyHealthBarFg->setBrush(QBrush(Qt::red));
    enemyHealthBarFg->setPen(QPen(Qt::transparent));
    enemyHealthBarFg->setZValue(101);
    addItem(enemyHealthBarFg);

    // 敌人血量文字
    enemyHealthText = new QGraphicsTextItem();
    enemyHealthText->setPos(enemyBarX, enemyBarY + HEALTH_BAR_HEIGHT + 5);
    enemyHealthText->setDefaultTextColor(Qt::white);
    enemyHealthText->setFont(QFont("Arial", 12, QFont::Bold));
    enemyHealthText->setZValue(102);
    addItem(enemyHealthText);
}

// 新增：更新血条UI
void BattleScene::updateHealthBars() {
    if (!character || !enemy) {
        return;
    }

    // 更新角色血条
    int characterCurrentHealth = character->getHealth();
    int characterMaxHealth = character->getMaxHealth();
    qreal characterHealthRatio = static_cast<qreal>(characterCurrentHealth) / characterMaxHealth;

    // 更新角色血条宽度
    QRectF characterRect = characterHealthBarFg->rect();
    characterRect.setWidth(HEALTH_BAR_WIDTH * characterHealthRatio);
    characterHealthBarFg->setRect(characterRect);

    // 更新角色血量文字
    characterHealthText->setPlainText(QString("Player: %1/%2").arg(characterCurrentHealth).arg(characterMaxHealth));

    // 更新敌人血条
    int enemyCurrentHealth = enemy->getHealth();
    int enemyMaxHealth = enemy->getMaxHealth();
    qreal enemyHealthRatio = static_cast<qreal>(enemyCurrentHealth) / enemyMaxHealth;

    // 更新敌人血条宽度
    QRectF enemyRect = enemyHealthBarFg->rect();
    enemyRect.setWidth(HEALTH_BAR_WIDTH * enemyHealthRatio);
    enemyHealthBarFg->setRect(enemyRect);

    // 更新敌人血量文字
    enemyHealthText->setPlainText(QString("Enemy: %1/%2").arg(enemyCurrentHealth).arg(enemyMaxHealth));

    // 根据血量变化血条颜色
    if (characterHealthRatio > 0.6) {
        characterHealthBarFg->setBrush(QBrush(Qt::green));
    } else if (characterHealthRatio > 0.3) {
        characterHealthBarFg->setBrush(QBrush(Qt::yellow));
    } else {
        characterHealthBarFg->setBrush(QBrush(Qt::red));
    }

    if (enemyHealthRatio > 0.6) {
        enemyHealthBarFg->setBrush(QBrush(Qt::green));
    } else if (enemyHealthRatio > 0.3) {
        enemyHealthBarFg->setBrush(QBrush(Qt::yellow));
    } else {
        enemyHealthBarFg->setBrush(QBrush(Qt::red));
    }
}

// 【新增】初始化护甲UI
void BattleScene::initArmorDisplays() {
    // 角色护甲文本 (左侧，血条下方)
    characterArmorText = new QGraphicsTextItem();
    // 定位在血量文字下方，留出一些间距
    characterArmorText->setPos(20, 20 + HEALTH_BAR_HEIGHT + 25);
    characterArmorText->setDefaultTextColor(Qt::cyan); // 使用与血量不同的颜色以区分
    characterArmorText->setFont(QFont("Arial", 12, QFont::Bold));
    characterArmorText->setZValue(102); // 与血量文字同层级
    addItem(characterArmorText);

    // 敌人护甲文本 (右侧，血条下方)
    enemyArmorText = new QGraphicsTextItem();
    // 定位在血量文字下方
    enemyArmorText->setPos(sceneRect().width() - HEALTH_BAR_WIDTH - 70, 20 + HEALTH_BAR_HEIGHT + 25);
    enemyArmorText->setDefaultTextColor(Qt::cyan);
    enemyArmorText->setFont(QFont("Arial", 12, QFont::Bold));
    enemyArmorText->setZValue(102);
    addItem(enemyArmorText);
}

// 【新增】更新护甲UI
void BattleScene::updateArmorDisplays() {
    if (!character || !enemy) {
        return;
    }

    // --- 更新角色护甲信息 ---
    Armor* characterArmor = character->getArmor(); // 使用我们之前添加的 getArmor()
    if (characterArmor) {
        // 如果有护甲，显示名称和耐久度
        QString armorInfo = QString("Armor: %1 (%2/%3)")
                                .arg(characterArmor->getName()) // 调用 getName()
                                .arg(characterArmor->getDurability())
                                .arg(characterArmor->getMaxDurability());
        characterArmorText->setPlainText(armorInfo);
    } else {
        // 如果没有护甲，显示 "None"
        characterArmorText->setPlainText("Armor: None");
    }

    // --- 更新敌人护甲信息 (逻辑完全相同) ---
    Armor* enemyArmor = enemy->getArmor();
    if (enemyArmor) {
        QString armorInfo = QString("Armor: %1 (%2/%3)")
        .arg(enemyArmor->getName())
            .arg(enemyArmor->getDurability())
            .arg(enemyArmor->getMaxDurability());
        enemyArmorText->setPlainText(armorInfo);
    } else {
        enemyArmorText->setPlainText("Armor: None");
    }
}

void BattleScene::processCharacterPicking(Character* aCharacter) {
    if (!aCharacter || !aCharacter->isPicking()) {
        return;
    }

    // 1. 寻找最近的可拾取物品
    Mountable* mountableToPick = findNearestUnmountedMountable(aCharacter->pos(), 100.0);
    if (mountableToPick == nullptr) {
        return;
    }

    // 2. 根据物品类型，执行不同的拾取逻辑
    // 【重要】将 mountableToPick 转换为 QGraphicsItem* 以便后续操作
    auto itemToPick = dynamic_cast<QGraphicsItem*>(mountableToPick);
    if (itemToPick == nullptr) {
        return; // 如果一个 Mountable 不能被转换为 QGraphicsItem，我们无法处理它
    }


    // 情况一：拾取的是消耗品 (Consumable)
    if (auto consumable = dynamic_cast<Consumable *>(mountableToPick)) {
        qDebug() << "Picking up a consumable.";
        // 消耗品：使用效果，然后从场景中移除并删除
        consumable->takeEffect(aCharacter);
        removeItem(itemToPick);
        delete itemToPick; // 直接删除 QGraphicsItem* 即可
    }
    // 情况二：拾取的是护甲 (Armor)
    else if (auto armor = dynamic_cast<Armor *>(mountableToPick)) {
        qDebug() << "Picking up an armor.";
        // 护甲：调用 pickupMountable，它会处理装备并返回旧护甲
        Mountable* oldMountable = pickupMountable(aCharacter, mountableToPick);

        // 如果有旧护甲被替换下来
        if (oldMountable) {
            if (auto oldItem = dynamic_cast<QGraphicsItem*>(oldMountable)) {
                // 将旧护甲丢在当前新护甲的位置
                oldItem->setPos(itemToPick->pos());
                oldMountable->unmount(); // 确保其可被再次拾取
                qDebug() << "Dropped old armor at" << oldItem->pos();
            }
        }
    }
    // 情况三：拾取的是武器 (Weapon)
    else if (auto weapon = dynamic_cast<Weapon *>(mountableToPick)) {
        qDebug() << "Picking up a weapon.";
        // 武器：逻辑与护甲完全相同
        Mountable* oldMountable = pickupMountable(aCharacter, mountableToPick);

        // 如果有旧武器被替换下来
        if (oldMountable) {
            if (auto oldItem = dynamic_cast<QGraphicsItem*>(oldMountable)) {
                // 将旧武器丢在当前新武器的位置
                oldItem->setPos(itemToPick->pos());
                oldMountable->unmount(); // 确保其可被再次拾取
                qDebug() << "Dropped old weapon at" << oldItem->pos();
            }
        }
    }
    // 注意：这里的 if/else if 结构确保了每种物品只被处理一次。
}

// 【改动】重构 processPicking 以调用新的辅助函数
void BattleScene::processPicking() {
    Scene::processPicking(); // 调用基类方法

    // 为玩家1处理拾取
    processCharacterPicking(character);

    // 为玩家2处理拾取
    processCharacterPicking(enemy);
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
    if (!character || !mountable) {
        return nullptr;
    }

    // 【修改】处理护甲拾取逻辑
    if (auto armor = dynamic_cast<Armor *>(mountable)) {
        // 调用 character->pickupArmor，它会正确地装备新护甲并返回旧护甲。
        // 这个返回值就是被替换下来的护甲，我们需要将它返回给调用者（processCharacterPicking）。
        return character->pickupArmor(armor);
    }
    // 处理武器拾取逻辑 (保持不变)
    else if (auto weapon = dynamic_cast<Weapon *>(mountable)) {
        Weapon* oldWeapon = character->getWeapon();
        character->setWeapon(weapon);
        qDebug() << "Character equipped weapon with attack power:" << weapon->getAttackPower();
        return oldWeapon;
    }

    return nullptr;
}

// 新增：消耗品掉落处理函数
void BattleScene::processConsumableDrop() {
    // 增加帧计数器
    consumableDropFrameCounter++;
    
    // 检查是否到了掉落消耗品的时间（每900帧）
    if (consumableDropFrameCounter >= CONSUMABLE_DROP_INTERVAL) {
        // 重置计数器
        consumableDropFrameCounter = 0;
        
        // 创建随机消耗品
        Consumable* newConsumable = createRandomConsumable();
        if (newConsumable) {
            // 设置消耗品的随机X位置（屏幕顶部）
            qreal randomX = QRandomGenerator::global()->bounded(static_cast<int>(50), static_cast<int>(sceneRect().width() - 50));
            newConsumable->setPos(randomX, 0);  // Y轴为0（屏幕顶部）
            
            // 添加到场景中
            addItem(newConsumable);
            
            // 添加到下落消耗品列表中
            fallingConsumables.append(newConsumable);
            
            qDebug() << "Consumable dropped at position:" << randomX << ", 0";
        }
    }
    
    // 更新所有正在下落的消耗品
    updateFallingConsumables();
}

// 新增：创建随机消耗品
Consumable* BattleScene::createRandomConsumable() {
    // 随机选择消耗品类型（0-2，对应Bandage、Medkit和Adrenaline）
    int consumableType = QRandomGenerator::global()->bounded(3);
    
    Consumable* consumable = nullptr;
    switch (consumableType) {
        case 0:
            consumable = new Bandage();
            qDebug() << "Created Bandage consumable";
            break;
        case 1:
            consumable = new Medkit();
            qDebug() << "Created Medkit consumable";
            break;
        case 2:
            consumable = new Adrenaline();
            qDebug() << "Created Adrenaline consumable";
            break;
        default:
            consumable = new Bandage();  // 默认创建绷带
            break;
    }
    
    return consumable;
}

// 新增：更新正在下落的消耗品
void BattleScene::updateFallingConsumables() {
    // 使用迭代器遍历，以便安全地删除元素
    auto it = fallingConsumables.begin();
    while (it != fallingConsumables.end()) {
        Consumable* consumable = *it;
        
        // 更新消耗品位置（向下移动）
        QPointF currentPos = consumable->pos();
        QPointF newPos = currentPos + QPointF(0, CONSUMABLE_FALL_SPEED);
        
        // 检查消耗品是否落到地面上
        bool hasLanded = false;
        if (map) {
            Platform* groundPlatform = map->getGroundPlatform(newPos, consumable->boundingRect().height());
            if (groundPlatform) {
                qreal surfaceY = groundPlatform->getSurfaceY();
                qreal consumableBottom = newPos.y() + consumable->boundingRect().height();
                
                // 如果消耗品底部接触或穿过平台表面
                if (consumableBottom >= surfaceY) {
                    // 将消耗品放置在平台表面上
                    newPos.setY(surfaceY - consumable->boundingRect().height());
                    hasLanded = true;
                }
            }
        }
        
        // 检查消耗品是否落到场景底部
        if (newPos.y() + consumable->boundingRect().height() >= sceneRect().height() - 120) {
            newPos.setY(sceneRect().height() - 120 - consumable->boundingRect().height());
            hasLanded = true;
        }
        
        // 更新消耗品位置
        consumable->setPos(newPos);
        
        // 如果消耗品已经着陆，从下落列表中移除
        if (hasLanded) {
            qDebug() << "Consumable landed at position:" << newPos;
            it = fallingConsumables.erase(it);
        } else {
            ++it;
        }
    }
}


// 【新增】游戏结束时显示胜利信息的函数
void BattleScene::showGameOverScreen(const QString &winnerName) {
    if (m_isGameOver) {
        return; // 防止重复调用
    }

    m_isGameOver = true; // 标记游戏已结束

    // 设置文本内容
    m_gameOverText->setPlainText(winnerName);

    // 将文本居中显示
    QRectF textRect = m_gameOverText->boundingRect();
    qreal x = (sceneRect().width() - textRect.width()) / 2.0;
    qreal y = (sceneRect().height() - textRect.height()) / 2.0;
    m_gameOverText->setPos(x, y);

    // 显示文本
    m_gameOverText->setVisible(true);

    qDebug() << "Game Over." << winnerName;
}
