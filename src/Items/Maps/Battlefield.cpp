// src/Items/Maps/Battlefield.cpp

#include "Battlefield.h"
#include <QGraphicsScene>

// 包含所有需要用到的平台类型的头文件
#include "platform/EarthPlatform.h"
#include "platform/GrassPlatform.h"
#include "platform/IcePlatform.h"

/**
 * @brief Battlefield 的构造函数.
 * 它的核心任务是创建所有平台实例，设置它们的位置，
 * 并将它们添加到场景和基类的平台列表中。
 */
Battlefield::Battlefield(QGraphicsScene* scene, QGraphicsItem *parent)
    // 1. 调用基类Map的构造函数，传入背景图路径
    : Map(parent, ":/MapBG2.png")
{
    // 确保传入的 scene 指针是有效的
    if (!scene) {
        return;
    }

    // --- 在这里进行关卡布局 ---

    // 2. 创建最底层的土地平台
    // 注意：这里的坐标 (0, 600) 等需要你根据自己的背景图和场景大小进行微调
    auto earthPlatform = new EarthPlatform();
    earthPlatform->setPos(0, 600); // 设置它在场景中的位置 (x, y)
    earthPlatform->setVisualWidth(1350);
    scene->addItem(earthPlatform); // 【重要】将平台添加到场景中以显示出来
    m_platforms.append(earthPlatform); // 将平台添加到基类的列表中进行逻辑管理

    // 3. 创建往上偏左的草地平台
    auto grassPlatform = new GrassPlatform();
    grassPlatform->setPos(0, 450);
    grassPlatform->setVisualWidth(600);
    scene->addItem(grassPlatform);
    m_platforms.append(grassPlatform);

    // 4. 创建往上偏右的冰场平台
    auto icePlatform = new IcePlatform();
    // 假设冰块平台图片较宽，为了居中显示，可以调整X坐标
    icePlatform->setPos(750,300);
    icePlatform->setVisualWidth(600);
    scene->addItem(icePlatform);
    m_platforms.append(icePlatform);
}

// 【移除】getFloorHeight 的实现函数被彻底删除
/*
qreal Battlefield::getFloorHeight() {
    auto sceneRect = sceneBoundingRect();
    return (sceneRect.top() + sceneRect.bottom()) * 0.63;
}
*/
