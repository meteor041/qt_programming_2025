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

// 【核心改动】实现 getGroundPlatform 函数  ！！！注意这个函数只是返回脚下最高的平台，不一定代表角色就在平台上
Platform* Map::getGroundPlatform(const QPointF& characterPos, qreal characterHeight) const {
    const qreal characterWidth = 50.0; // 重要：这个值应与你角色的实际宽度匹配！

    Platform* groundCandidate = nullptr;
    // 初始化一个极大的Y值。我们会寻找Y值比它小的平台。
    qreal highestSurfaceY = 1e9;

    QRectF characterRect(characterPos, QSizeF(characterWidth, characterHeight));
    qreal characterBottom = characterRect.bottom();
    const qreal LANDING_TOLERANCE = 2.0;

    // 遍历所有平台
    for (Platform* p : m_platforms) {
        QRectF platformRect = p->sceneBoundingRect();

        // --- 第1步：过滤 ---
        // 检查平台是否是一个有效的“地面候选者”。

        // 条件 A: 角色和平台在水平方向上必须有重叠。
        bool horizontalOverlap = (characterRect.left() < platformRect.right() &&
                                  characterRect.right() > platformRect.left());

        // 条件 B: 平台的表面必须在角色脚的下方（或在容差范围内）。
        // 这会将所有在角色头顶上方的平台排除掉。
        bool isPlatformBelowFeet = (characterBottom <= platformRect.top() + LANDING_TOLERANCE);

        if (horizontalOverlap && isPlatformBelowFeet) {
            // 如果一个平台通过了过滤，它就是一个“候选者”。

            // --- 第2步：选择 ---
            // 从所有候选者中，选出最靠上的那一个（Y坐标最小）。
            if (platformRect.top() < highestSurfaceY) {
                // 我们找到了一个比之前记录的更靠上的平台。
                // 更新我们的“最佳选择”。
                highestSurfaceY = platformRect.top();
                groundCandidate = p;
            }
        }
    }

    // 循环结束后，groundCandidate 要么是nullptr（脚下没有任何平台），
    // 要么指向那个离角色最近的下方平台。
    return groundCandidate;
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
