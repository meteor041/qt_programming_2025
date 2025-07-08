//
// Created by gerw on 8/20/24.
//

#ifndef QT_PROGRAMMING_2024_ITEM_H
#define QT_PROGRAMMING_2024_ITEM_H

#include <QGraphicsItem>
#include <QPainter>

class Item : public QGraphicsItem {
public:
    //构造函数
    explicit Item(QGraphicsItem *parent, const QString &pixmapPath);

    //生成矩形框
    [[nodiscard]] QRectF boundingRect() const override {
        if (pixmapItem != nullptr) {
            return this->pixmapItem->boundingRect();
        }
        // Empty rectangle makes the empty item not selectable and not collider
        return {};
    }

    //渲染
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override {
        // Draw nothing as it is an empty item
        // Its pixmapItem is automatically handled by children's paint()
    }

    //用于显示图形的指针
protected:
    QGraphicsPixmapItem *pixmapItem{};
};


#endif //QT_PROGRAMMING_2024_ITEM_H
