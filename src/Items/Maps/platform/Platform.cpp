// src/Items/Maps/platform/Platform.cpp

#include "Platform.h"
#include <QTransform> // 需要包含 QTransform 头文件
#include <QPainter>

Platform::Platform(const QString& pixmapPath, QGraphicsItem *parent) : QGraphicsItem(parent) {
    m_pixmap = QPixmap(pixmapPath);
    
    // 【核心改动】记录原始图片和默认宽度
    m_originalWidth = m_pixmap.width();
    m_currentWidth = m_originalWidth; // 默认宽度为原始宽度
}

// 【核心改动】实现设置视觉宽度的方法 - 改为复制排列
void Platform::setVisualWidth(qreal width) {
    if (width > 0) {
        m_currentWidth = width;
        // 不再使用变换，而是在paint函数中处理复制排列
        update(); // 触发重绘
    }
}

// getSurfaceY 保持不变
qreal Platform::getSurfaceY() const {
    return sceneBoundingRect().top();
}

// 【核心改动】修改boundingRect以反映当前宽度
QRectF Platform::boundingRect() const {
    return QRectF(0, 0, m_currentWidth, m_pixmap.height());
}

// 【核心改动】实现复制排列的绘制逻辑
void Platform::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    if (m_pixmap.isNull()) {
        return;
    }
    
    // 计算需要绘制多少个完整的图片
    int fullTiles = static_cast<int>(m_currentWidth / m_originalWidth);
    qreal remainingWidth = m_currentWidth - (fullTiles * m_originalWidth);
    
    // 绘制完整的图片瓦片
    for (int i = 0; i < fullTiles; ++i) {
        qreal x = i * m_originalWidth;
        painter->drawPixmap(x, 0, m_pixmap);
    }
    
    // 如果有剩余宽度，绘制部分图片
    if (remainingWidth > 0) {
        qreal x = fullTiles * m_originalWidth;
        QRect sourceRect(0, 0, static_cast<int>(remainingWidth), m_pixmap.height());
        QRect targetRect(static_cast<int>(x), 0, static_cast<int>(remainingWidth), m_pixmap.height());
        painter->drawPixmap(targetRect, m_pixmap, sourceRect);
    }
}

void Platform::onCharacterEnter(Character* character) {}
void Platform::onCharacterStay(Character* character) {}
void Platform::onCharacterLeave(Character* character) {}
