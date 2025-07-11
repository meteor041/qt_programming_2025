// src/Scenes/BattleScene.h

#ifndef QT_PROGRAMMING_2024_BATTLESCENE_H
#define QT_PROGRAMMING_2024_BATTLESCENE_H

#include <QKeyEvent>
#include <QRandomGenerator>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include "Scene.h"
#include "../Items/Maps/Map.h"
#include "../Items/Characters/Character.h"
#include "../Items/Armors/Armor.h" // 包含完整的Armor定义
// 添加武器类的包含
#include "../Items/Weapon/Fist.h"
#include "../Items/Weapon/Knife.h"
#include "../Items/Weapon/ShotPut.h"
// 添加消耗品类的包含
#include "../Items/Consumables/Consumable.h"
#include "../Items/Consumables/Bandage.h"
#include "../Items/Consumables/Medkit.h"
#include "../Items/Consumables/Adrenaline.h"
#include <QElapsedTimer>
#include "../Items/Armors/ChainmailArmor.h"
#include "../Items/Armors/BulletproofVest.h"

class BattleScene : public Scene {
    Q_OBJECT

public:
    explicit BattleScene(QObject *parent);

    void processInput() override;
    void processMovement() override;
    void processPicking() override;
    void processPhysics() override;
    
    // 新增：战斗处理函数
    void processCombat();
    
    // 新增：武器掉落处理函数
    void processWeaponDrop();

    void addProjectile(ShotPutProjectile* projectile);
    
    void processProjectiles();
    // 新增：消耗品掉落处理函数
    void processConsumableDrop();

    // 【新增】护甲掉落处理函数
    void processArmorDrop();

protected slots:
    void update() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    Mountable *findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold = std::numeric_limits<qreal>::max());
    static Mountable * pickupMountable(Character *character, Mountable *mountable);
    
    // 新增：战斗相关辅助函数
    bool isInAttackRange(Character* attacker, Character* target, qreal range = 100.0);
    
    // 新增：武器掉落相关辅助函数
    Weapon* createRandomWeapon();
    void updateFallingWeapons();
    
    // 新增：消耗品掉落相关辅助函数
    Consumable* createRandomConsumable();
    void updateFallingConsumables();

    // 【新增】护甲掉落相关辅助函数
    Armor* createRandomArmor();
    void updateFallingArmors();
    
    // 新增：血条UI相关函数
    void initHealthBars();
    void updateHealthBars();

    // 【新增】护甲UI相关函数
    void initArmorDisplays();
    void updateArmorDisplays();

    // 【新增】用于处理单个角色移动的辅助函数，避免代码重复
    void processCharacterMovement(Character* aCharacter);
    // 【新增】用于处理单个角色拾取的辅助函数
    void processCharacterPicking(Character* aCharacter);
    // 【新增】用于处理单个角色攻击逻辑的辅助函数
    void processCharacterCombat(Character* attacker, Character* target, bool& attackFlag);

    // 【新增】游戏结束相关辅助函数
    void showGameOverScreen(const QString& winnerName);

    // 【新增】用于显示FPS的成员变量
    QGraphicsTextItem *fpsTextItem;   // 用于显示FPS的文本项
    QElapsedTimer fpsTimer;           // 用于计时的计时器
    int frameCount{0};                // 帧计数器
    Map *map;
    Character *character;
    Character *enemy;

     // 新增：玩家1和玩家2的攻击按键状态
     bool attackKeyDown{false};      // 玩家1 (K键)
     bool enemyAttackKeyDown{false}; // 玩家2 (0键)
    // 新增：武器掉落相关成员变量
    int weaponDropFrameCounter{0};  // 帧计数器
    static const int WEAPON_DROP_INTERVAL = 900;  // 900帧 = 10秒
    QList<Weapon*> fallingWeapons;  // 正在下落的武器列表
    static constexpr qreal WEAPON_FALL_SPEED = 10.0;  // 武器下落速度
    
    // 新增：消耗品掉落相关成员变量
    int consumableDropFrameCounter{0};  // 消耗品帧计数器
    static const int CONSUMABLE_DROP_INTERVAL = 900;  // 900帧 = 15秒
    QList<Consumable*> fallingConsumables;  // 正在下落的消耗品列表
    static constexpr qreal CONSUMABLE_FALL_SPEED = 10.0;  // 消耗品下落速度
    
    // 新增：投掷物管理相关成员变量
    QList<ShotPutProjectile*> projectiles;  // 活跃的投掷物列表
    
    // 【新增】护甲掉落相关成员变量
    int armorDropFrameCounter{0};
    static const int ARMOR_DROP_INTERVAL = 1200; // 1200帧 ≈ 20秒
    QList<Armor*> fallingArmors;
    static constexpr qreal ARMOR_FALL_SPEED = 10.0;
    // 新增：血条UI相关成员变量

    QGraphicsRectItem* characterHealthBarBg;  // 角色血条背景
    QGraphicsRectItem* characterHealthBarFg;  // 角色血条前景
    QGraphicsTextItem* characterHealthText;   // 角色血量文字
    QGraphicsRectItem* enemyHealthBarBg;      // 敌人血条背景
    QGraphicsRectItem* enemyHealthBarFg;      // 敌人血条前景
    QGraphicsTextItem* enemyHealthText;       // 敌人血量文字
    
    // 【新增】护甲UI相关成员变量
    QGraphicsTextItem* characterArmorText;   // 角色护甲文字
    QGraphicsTextItem* enemyArmorText;       // 敌人护甲文字
    static constexpr qreal HEALTH_BAR_WIDTH = 200.0;   // 血条宽度
    static constexpr qreal HEALTH_BAR_HEIGHT = 20.0;   // 血条高度
    // 【新增】游戏结束状态和UI
    bool m_isGameOver{false};               // 游戏是否结束的状态标志
    QGraphicsTextItem* m_gameOverText{nullptr}; // 用于显示 "Game Over" 或胜利信息的文本项
};

#endif //QT_PROGRAMMING_2024_BATTLESCENE_H
