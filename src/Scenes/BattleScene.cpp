// src/Scenes/BattleScene.cpp

#include <QDebug>
#include <QLineF> // 用于计算距离
#include "BattleScene.h"
#include "../Items/Characters/Link.h" // 假设你的角色是Link
#include "../Items/Maps/Battlefield.h"
#include "../Items/Armors/FlamebreakerArmor.h"
#include "../Items/Maps/platform/Platform.h" // 包含平台基类
#include "../Items/Weapon/ShotPut.h" // 包含投掷物类

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
    // 【改动】使用局部变量创建初始盔甲，不再使用成员变量 spareArmor
    Armor* initialArmor = new FlamebreakerArmor();
    addItem(initialArmor);
    initialArmor->unmount();

    // 将盔甲放置在场景中央的平台上
    QPointF armorTargetPos(sceneRect().center().x() + 200, 0); // 设定一个目标水平位置
    Platform* groundForArmor = map->getGroundPlatform(armorTargetPos, initialArmor->boundingRect().height());
    if (groundForArmor) {
        // 如果找到了平台，将盔甲放在平台表面上方
        initialArmor->setPos(armorTargetPos.x(),
                             groundForArmor->getSurfaceY() - initialArmor->boundingRect().height());
    } else {
        // 如果没找到平台（作为备用方案），放在一个安全的位置
        initialArmor->setPos(sceneRect().center());
    }
    
    // 初始化血条UI
    initHealthBars();
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
                // qDebug() << "vertical overlap"; // 如果需要调试，可以取消此行注释
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
                        // 将角色位置限制在平台右侧
                        newPos.setX(platformRight);
                        // 反转X轴速度
                        aCharacter->setVelocity(QPointF(-aCharacter->getVelocity().x(), aCharacter->getVelocity().y()));
                        break; // 找到碰撞就停止检查其他平台
                    }
                }
            }
        }
    }

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
        // qDebug() << "processCharacterCombat: Invalid parameters or attacker is dead";
        return;
    }

    // 检查目标是否在攻击范围内
    Weapon* weapon = attacker->getWeapon();
    if (!weapon) {
        qDebug() << "No weapon equipped!";
        return;
    }
    // 根据武器类型执行不同的攻击逻辑
    if (dynamic_cast<Fist*>(weapon) || dynamic_cast<Knife*>(weapon)) {
        if (isInAttackRange(attacker, target, attacker->getWeaponAttackRange())) {
            // 执行攻击动画/效果
            attacker->performAttack();

            // 从武器获取伤害值
            int damage = attacker->getWeapon()->getAttackPower();
            target->takeDamage(damage);

            qDebug() << "Character attacked Character"
                    << "for" << damage << "damage! Target health:" << target->getHealth();
        } else {
            qDebug() << "Character tried to attack, but target is too far!";
        }
    } else if (dynamic_cast<ShotPut*>(weapon)) {
        // ShotPut 只执行 performAttack
        attacker->performAttack();
    }
    // 重置攻击状态，实现单次按下、单次攻击
    attackFlag = false;
}

// BattleScene.cpp

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


// 在现有的update()方法中添加武器掉落处理
void BattleScene::update() {
    // 添加战斗处理
    processCombat();
    // 添加武器掉落处理
    processWeaponDrop();
    // 添加消耗品掉落处理
    processConsumableDrop();
    // 添加投掷物处理
    processProjectiles();
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

// 新增：创建随机武器
// 在 createRandomWeapon() 函数中修改
Weapon* BattleScene::createRandomWeapon() {
    // 随机选择武器类型（现在包括实心球）
    int weaponType = QRandomGenerator::global()->bounded(2);  // 0-2，对应Fist、Knife、ShotPut
    
    Weapon* weapon = nullptr;
    switch (weaponType) {
        // case 0:
        //     weapon = new Fist();
        //     qDebug() << "Created Fist weapon";
        //     break;
        case 0:
            weapon = new Knife();
            qDebug() << "Created Knife weapon";
            break;
        case 1:
            weapon = new ShotPut(nullptr, 3); // 3次投掷
            qDebug() << "Created ShotPut weapon";
            break;
        default:
            weapon = new Fist();
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
    qreal enemyBarX = sceneRect().width() - HEALTH_BAR_WIDTH - 20;
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
void BattleScene::processCharacterPicking(Character* aCharacter) {
    if (!aCharacter || !aCharacter->isPicking()) {
        return;
    }

    auto mountable = findNearestUnmountedMountable(aCharacter->pos(), 100.0);
    if (mountable != nullptr) {
        // 检查是否是盔甲
        if (auto armor = dynamic_cast<Armor *>(mountable)) {
            // pickupMountable 会返回旧盔甲，旧盔甲会自动 unmount 并留在原地。
            // 我们不需要用任何变量来接住它。
            pickupMountable(aCharacter, mountable);
        }
        // 检查是否是武器
        else if (auto weapon = dynamic_cast<Weapon *>(mountable)) {
            // 拾取武器，如果角色之前有武器，旧武器会被放置在当前位置
            Weapon* oldWeapon = dynamic_cast<Weapon *>(pickupMountable(aCharacter, mountable));
            if (oldWeapon) {
                // 将旧武器放置在拾取新武器的位置
                oldWeapon->setPos(dynamic_cast<QGraphicsItem*>(mountable)->pos());
                oldWeapon->unmount(); // 确保它回到可拾取状态
                qDebug() << "Dropped old weapon at position:" << oldWeapon->pos();


            }
        }
        // 检查是否是消耗品
        else if (auto consumable = dynamic_cast<Consumable *>(mountable)) {
            // 使用消耗品
            consumable->takeEffect(aCharacter);
            // 从场景中移除消耗品
            removeItem(dynamic_cast<QGraphicsItem*>(consumable));
            delete consumable;
            qDebug() << "Consumable used by a character and removed from scene";
        }
    }
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
    if (auto armor = dynamic_cast<Armor *>(mountable)) {
        return character->pickupArmor(armor);
    }
    // 新增：处理武器拾取
    else if (auto weapon = dynamic_cast<Weapon *>(mountable)) {
        // 获取角色当前装备的武器
        Weapon* oldWeapon = character->getWeapon();
        // 装备新武器
        character->setWeapon(weapon);
        qDebug() << "Character equipped weapon with attack power:" << weapon->getAttackPower();
        // 返回之前装备的武器（如果有的话）
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

// 新增：添加投掷物到管理列表
void BattleScene::addProjectile(ShotPutProjectile* projectile) {
    if (projectile) {
        projectiles.append(projectile);
        qDebug() << "Added projectile to BattleScene management, total:" << projectiles.size();
    }
}

// 新增：处理所有投掷物的更新和碰撞检测
void BattleScene::processProjectiles() {
    // 使用迭代器遍历，以便安全地删除元素
    auto it = projectiles.begin();
    while (it != projectiles.end()) {
        ShotPutProjectile* projectile = *it;
        
        // 检查投掷物是否仍然有效
        if (!projectile) {
            // 投掷物已被删除或不在当前场景中，从列表中移除
            it = projectiles.erase(it);
            continue;
        }
         // 检查是否标记为删除
        if (projectile->isMarkedForDeletion()) {
            removeItem(projectile);
            delete projectile;
            it = projectiles.erase(it);
            continue;
        }
        // 更新投掷物位置
        projectile->updatePosition();
        
        // 检查碰撞
        projectile->checkCollision();
        
        ++it;
    }
}
