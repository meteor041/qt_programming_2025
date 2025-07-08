#ifndef PLATFORM_H
#define PLATFORM_H

#include <QGraphicsItem>
#include <QPixmap>

// 前向声明
class Character;

class Platform : public QGraphicsItem {
public:
    // 构造函数现在接收一个贴图路径
    explicit Platform(const QString& pixmapPath, QGraphicsItem *parent = nullptr);

    void setVisualWidth(qreal width);
    // 获取平台表面的Y坐标 (相对于平台自身原点)
    qreal getSurfaceY() const;

    // --- 碰撞和渲染相关的重写 ---
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    // --- 多态的核心 (保持不变) ---
    virtual void onCharacterEnter(Character* character);
    virtual void onCharacterStay(Character* character);
    virtual void onCharacterLeave(Character* character);

protected:
    QPixmap m_pixmap;        // 原始图片
    qreal m_originalWidth;   // 原始图片宽度
    qreal m_currentWidth;    // 当前平台宽度
};

#endif // PLATFORM_H
