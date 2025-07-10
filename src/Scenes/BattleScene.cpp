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
    
    // 初始化血条UI
    initHealthBars();
}

// 【核心改动 #3】实现物理处理逻辑 (修正版)
// 【推荐】创建一个辅助函数（未在头文件声明，仅用于本文件）来处理单个角色的物理和平台事件，避免对于不同角色代码重复
void processCharacterPhysicsAndEvents(Character* character, Map* map) {
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
                bool verticalOverlap = newCharRect.intersects(platformRect);
                
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
                    qDebug() << "vertical overlap";
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
            enemy->takeDamage(character->getWeaponAttackPower());
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
Weapon* BattleScene::createRandomWeapon() {
    // 随机选择武器类型（不包括Weapon基类）
    int weaponType = QRandomGenerator::global()->bounded(2);  // 0-1，对应Fist和Knife
    
    Weapon* weapon = nullptr;
    switch (weaponType) {
        case 0:
            weapon = new Fist();
            qDebug() << "Created Fist weapon";
            break;
        case 1:
            weapon = new Knife();
            qDebug() << "Created Knife weapon";
            break;
        default:
            weapon = new Fist();  // 默认创建拳头
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

void BattleScene::processPicking() {
    Scene::processPicking();
    if (character && character->isPicking()) {
        auto mountable = findNearestUnmountedMountable(character->pos(), 100.);
        qDebug() << mountable;
        if (mountable != nullptr) {
            // 检查是否是盔甲
            if (auto armor = dynamic_cast<Armor *>(mountable)) {
                spareArmor = dynamic_cast<Armor *>(pickupMountable(character, mountable));
            }
            // 检查是否是武器
            else if (auto weapon = dynamic_cast<Weapon *>(mountable)) {
                // 拾取武器，如果角色之前有武器，旧武器会被放置在当前位置
                Weapon* oldWeapon = dynamic_cast<Weapon *>(pickupMountable(character, mountable));
                // if (oldWeapon) {
                //     // 将旧武器放置在拾取位置
                //     oldWeapon->setPos(dynamic_cast<QGraphicsItem*>(mountable)->pos());
                //     qDebug() << "Dropped old weapon at position:" << dynamic_cast<QGraphicsItem*>(mountable)->pos();
                // }
            }
            // 检查是否是消耗品
            else if (auto consumable = dynamic_cast<Consumable *>(mountable)) {
                // 使用消耗品
                consumable->takeEffect(character);
                // 从场景中移除消耗品
                removeItem(dynamic_cast<QGraphicsItem*>(consumable));
                delete consumable;
                qDebug() << "Consumable used and removed from scene";
            }
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
