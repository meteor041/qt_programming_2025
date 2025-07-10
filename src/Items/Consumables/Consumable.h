#ifndef QT_PROGRAMMING_2024_CONSUMABLE_H
#define QT_PROGRAMMING_2024_CONSUMABLE_H

#include "../Item.h"
#include "../Mountable.h"

// 前向声明，避免循环依赖
class Character;

class Consumable : public Item, public Mountable {
public:
    explicit Consumable(QGraphicsItem *parent, const QString &pixmapPath);
    
    // 纯虚函数，子类必须实现具体效果
    virtual void takeEffect(Character* character) = 0;
    
    // 虚析构函数
    virtual ~Consumable() = default;
};

#endif //QT_PROGRAMMING_2024_CONSUMABLE_H