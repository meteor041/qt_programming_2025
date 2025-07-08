// src/Items/Maps/Battlefield.h

#ifndef QT_PROGRAMMING_2024_BATTLEFIELD_H
#define QT_PROGRAMMING_2024_BATTLEFIELD_H

#include "Map.h"

class Battlefield: public Map {
public:
    /**
     * @brief 构造战场地图.
     * @param scene 指向游戏场景的指针，用于将平台添加到场景中.
     * @param parent 父QGraphicsItem.
     */
    explicit Battlefield(QGraphicsScene* scene, QGraphicsItem *parent = nullptr);

    // 【移除】不再需要重写 getFloorHeight
    // qreal getFloorHeight() override;
};

#endif //QT_PROGRAMMING_2024_BATTLEFIELD_H
