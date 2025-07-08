// src/Scenes/BattleScene.h

#ifndef QT_PROGRAMMING_2024_BATTLESCENE_H
#define QT_PROGRAMMING_2024_BATTLESCENE_H

#include <QKeyEvent>
#include <QList>
#include "Scene.h"
#include "../Items/Maps/Map.h"
#include "../Items/Characters/Character.h"
#include "../Items/Armors/Armor.h"
#include "../Items/Mountable.h" // [FIXED] 确保包含Mountable

class BattleScene : public Scene {
    Q_OBJECT

public:
    explicit BattleScene(QObject *parent);

    // [MODIFIED] 重命名函数以明确职责
    void processCharacterUpdates() override;
    void processPicking() override;
    void processPhysics() override;

protected slots:
    void update() override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    Mountable *findNearestUnmountedMountable(const QPointF &pos, qreal distance_threshold = 100.0);
    static Mountable * pickupMountable(Character *character, Mountable *mountable);

    Map *map;
    // [MODIFIED] 支持双玩家
    Character *character1;
    Character *character2;

    // 示例物品列表，方便管理
    QList<QGraphicsItem*> worldItems;
};

#endif //QT_PROGRAMMING_2024_BATTLESCENE_H
