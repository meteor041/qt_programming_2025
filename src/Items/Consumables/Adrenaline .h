#ifndef QT_PROGRAMMING_2024_ADRENALINE_H
#define QT_PROGRAMMING_2024_ADRENALINE_H

#include "Consumable.h"

class Adrenaline : public Consumable {
public:
    explicit Adrenaline(QGraphicsItem *parent = nullptr);
    
    // 实现基类的纯虚函数
    void takeEffect(Character* character) override;
    
private:
    static const int EFFECT_DURATION = 600; // 持续10秒（60fps * 10秒）
    static const qreal SPEED_MULTIPLIER = 1.5; // 速度增加50%
    static const int HEAL_PER_FRAME = 1; // 每60帧回血1点
};

#endif //QT_PROGRAMMING_2024_ADRENALINE_H