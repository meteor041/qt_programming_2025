#ifndef PLATFORM_H
#define PLATFORM_H

#include <QGraphicsPixmapItem>

// 前向声明
class Character;

// 不再继承 QGraphicsRectItem，而是继承我们自己的 Item 基类，因为它已经处理了图片显示
// 或者，如果不想引入 Item，我们可以直接继承 QGraphicsItem 并自己管理 pixmap
// 这里我们选择后者，让 Platform 保持独立
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
    QGraphicsPixmapItem* m_pixmapItem; // 用于显示平台的图片

private:
    qreal m_originalWidth;
};

#endif // PLATFORM_H
