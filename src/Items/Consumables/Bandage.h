#ifndef QT_PROGRAMMING_2024_BANDAGE_H
#define QT_PROGRAMMING_2024_BANDAGE_H

#include "Consumable.h"

class Bandage : public Consumable {
public:
    explicit Bandage(QGraphicsItem *parent = nullptr);
    
    // 实现基类的纯虚函数
    void takeEffect(Character* character) override;
};

#endif //QT_PROGRAMMING_2024_BANDAGE_H