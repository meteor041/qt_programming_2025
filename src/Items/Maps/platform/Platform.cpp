// src/Items/Maps/platform/Platform.cpp

#include "Platform.h"
#include <QTransform> // 需要包含 QTransform 头文件

Platform::Platform(const QString& pixmapPath, QGraphicsItem *parent) : QGraphicsItem(parent) {
    m_pixmapItem = new QGraphicsPixmapItem(QPixmap(pixmapPath), this);

    // 【核心改动】在构造函数中，记录下图片的原始宽度
    m_originalWidth = m_pixmapItem->boundingRect().width();
}

// 【核心改动】实现设置视觉宽度的方法
void Platform::setVisualWidth(qreal width) {
    // 如果原始宽度有效，则进行计算
    if (m_originalWidth > 0) {
        // 计算X方向的缩放比例
        qreal scaleX = width / m_originalWidth;

        // 创建一个只在X方向缩放的变换矩阵
        // Y方向的缩放比例为 1.0，表示不改变高度
        QTransform transform;
        transform.scale(scaleX, 1.0);

        // 将此变换应用到整个Platform对象上
        setTransform(transform);
    }
}

// getSurfaceY, boundingRect, paint 等其他函数保持不变
qreal Platform::getSurfaceY() const {
    return sceneBoundingRect().top();
}

QRectF Platform::boundingRect() const {
    return m_pixmapItem->boundingRect();
}

void Platform::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    // 无需绘制
}

void Platform::onCharacterEnter(Character* character) {}
void Platform::onCharacterStay(Character* character) {}
void Platform::onCharacterLeave(Character* character) {}
