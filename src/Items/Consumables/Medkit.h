#ifndef QT_PROGRAMMING_2024_MEDKIT_H
#define QT_PROGRAMMING_2024_MEDKIT_H

#include "Consumable.h"

class Medkit : public Consumable {
public:
    explicit Medkit(QGraphicsItem *parent = nullptr);
    
    // 实现基类的纯虚函数
    void takeEffect(Character* character) override;
};

#endif //QT_PROGRAMMING_2024_MEDKIT_H