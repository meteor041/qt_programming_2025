// src/Scenes/BattleScene.h

#ifndef QT_PROGRAMMING_2024_BATTLESCENE_H
#define QT_PROGRAMMING_2024_BATTLESCENE_H

#include <QKeyEvent>
#include <QRandomGenerator>
#include "Scene.h"
#include "../Items/Maps/Map.h"
#include "../Items/Characters/Character.h"
#include "../Items/Armors/Armor.h" // 包含完整的Armor定义
// 添加武器类的包含
#include "../Items/Weapon/Fist.h"
#include "../Items/Weapon/Knife.h"

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

    Map *map;
    Character *character;
    Character *enemy;
    Armor *spareArmor;
    
    // 新增：攻击按键状态
    bool attackKeyDown{false};
    
    // 新增：武器掉落相关成员变量
    int weaponDropFrameCounter{0};  // 帧计数器
    static const int WEAPON_DROP_INTERVAL = 900;  // 900帧 = 10秒
    QList<Weapon*> fallingWeapons;  // 正在下落的武器列表
    static constexpr qreal WEAPON_FALL_SPEED = 10.0;  // 武器下落速度
};

#endif //QT_PROGRAMMING_2024_BATTLESCENE_H
