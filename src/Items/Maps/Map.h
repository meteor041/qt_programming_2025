// src/Items/Maps/Map.h

#ifndef QT_PROGRAMMING_2024_MAP_H
#define QT_PROGRAMMING_2024_MAP_H

#include <QGraphicsScene>
#include <QPointF>
#include <QList>
#include "../Item.h"
#include "platform/Platform.h" // 包含平台基类的头文件

class Map : public Item {
public:
    // 构造函数保持不变
    explicit Map(QGraphicsItem *parent = nullptr, const QString &pixmapPath = "");

    // 新增析构函数，用于释放平台占用的内存
    virtual ~Map();

    // 画面缩放调整函数，保持不变
    void scaleToFitScene(QGraphicsScene *scene);

    // 【核心改动】新增函数：根据一个位置，获取其脚下最高的平台
    // const 保证这个函数不会修改Map的状态
    Platform* getGroundPlatform(const QPointF& position, qreal characterHeight = 50.0) const;

    // 【移除】这个函数的功能已经被 getGroundPlatform 替代，不再需要
    // virtual qreal getFloorHeight();

    // 出生点函数，我们将修改它的实现逻辑
    virtual QPointF getSpawnPos();

protected:
    // 【核心改动】用于存储本地图中所有平台的列表
    // protected 意味着子类 (Battlefield) 可以直接访问它
    QList<Platform*> m_platforms;
};

#endif //QT_PROGRAMMING_2024_MAP_H
