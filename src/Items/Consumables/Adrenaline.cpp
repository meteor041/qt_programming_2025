#include "Adrenaline.h"
#include "../Characters/Character.h"
#include <QDebug>

Adrenaline::Adrenaline(QGraphicsItem* parent) 
    : Consumable(parent, ":/adrenaline.svg") {
    // 构造函数，使用肾上腺素图片
}

void Adrenaline::takeEffect(Character* character) {
    if (!character) return;
    
    // 启动肾上腺素效果：持续时间、速度倍率、每帧回血量
    character->startAdrenalineEffect(ADRENALINE_DURATION, ADRENALINE_SPEED_MULTIPLIER, ADRENALINE_HEAL_PER_FRAME);
    
    qDebug() << "Adrenaline used! Character gains speed boost and health regeneration.";
}