// src/Scenes/BattleScene.h

#ifndef QT_PROGRAMMING_2024_BATTLESCENE_H
#define QT_PROGRAMMING_2024_BATTLESCENE_H

#include <QKeyEvent>
#include "Scene.h"
#include "../Items/Maps/Map.h"
#include "../Items/Characters/Character.h"
#include "../Items/Armors/Armor.h" // 包含完整的Armor定义

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

    Map *map;
    Character *character;
    Character *enemy;
    Armor *spareArmor;
    
    // 新增：攻击按键状态
    bool attackKeyDown{false};
};

#endif //QT_PROGRAMMING_2024_BATTLESCENE_H
