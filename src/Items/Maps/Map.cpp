// src/Items/Maps/Map.cpp

#include "Map.h"

// 构造函数不变
Map::Map(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath) {}

// 【核心改动】实现析构函数
Map::~Map() {
    // 使用 Qt 的 qDeleteAll 函数安全地删除列表中所有的平台指针，并清空列表
    // 这是防止内存泄漏的关键步骤！
    qDeleteAll(m_platforms);
    m_platforms.clear();
}

// scaleToFitScene 函数不变
void Map::scaleToFitScene(QGraphicsScene *scene) {
    QRectF sceneRect = scene->sceneRect();
    QRectF itemRect = boundingRect();
    qreal scaleX = sceneRect.width() / itemRect.width();
    qreal scaleY = sceneRect.height() / itemRect.height();
    qreal scaleFactor = qMin(scaleX, scaleY);
    setTransform(QTransform::fromScale(scaleFactor, scaleFactor), true);
    setPos((sceneRect.width() - boundingRect().width() * scaleFactor) / 2,
           (sceneRect.height() - boundingRect().height() * scaleFactor) / 2);
}

// 【核心改动】实现 getGroundPlatform 函数
Platform* Map::getGroundPlatform(const QPointF& position) const {
    Platform* ground = nullptr;
    qreal highestY = 1e9; // 初始化一个极大值，代表无穷远

    // 遍历地图中所有的平台
    for (Platform* p : m_platforms) {
        QRectF platformRect = p->sceneBoundingRect(); // 获取平台在场景中的矩形范围

        // 检查：1. 角色是否在平台的水平(X)范围内
        //       2. 角色的位置是否在平台的上方 (position.y() <= platformRect.top())
        if (position.x() >= platformRect.left() &&
            position.x() <= platformRect.right() &&
            position.y() >= platformRect.top() - 50) {
            qDebug() << position.y() << ", " << platformRect.top();
            // 如果找到一个符合条件的平台，我们还要判断它是不是“最高”的那个
            // （在屏幕坐标系中，Y值越小越靠上）
            if (platformRect.top() < highestY) {
                highestY = platformRect.top(); // 更新最高点
                ground = p; // 记录这个平台
            }
        }
    }
    return ground; // 返回找到的最高平台，如果脚下没有则返回 nullptr
}


// 【核心改动】修改 getSpawnPos 的实现
QPointF Map::getSpawnPos() {
    // 如果地图中没有任何平台，返回一个默认安全位置（左上角）
    if (m_platforms.isEmpty()) {
        return {100, 100};
    }

    // 默认在第一个平台上出生
    Platform* spawnPlatform = m_platforms.first();
    QRectF platformRect = spawnPlatform->sceneBoundingRect();

    // 出生在平台的中点顶部
    qreal spawnX = platformRect.center().x();
    qreal spawnY = platformRect.top(); // 之后角色会因为重力站稳在平台上

    return {spawnX, spawnY - 50}; // 在平台上方一点出生，然后自然下落
}
