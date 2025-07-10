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

    // 【新增】用于处理单个角色移动的辅助函数，避免代码重复
    void processCharacterMovement(Character* aCharacter);
    // 【新增】用于处理单个角色拾取的辅助函数
    void processCharacterPicking(Character* aCharacter);

    Map *map;
    Character *character;
    Character *enemy;
    // spareArmor 成员变量应该被删掉，因为它在双人模式下会引起逻辑混乱。
    // 现在我在做别的，就先不处理了
     Armor *spareArmor;

    
    // 新增：玩家1和玩家2的攻击按键状态
    bool attackKeyDown{false};      // 玩家1 (K键)
    bool enemyAttackKeyDown{false}; // 玩家2 (0键)
};

#endif //QT_PROGRAMMING_2024_BATTLESCENE_H
