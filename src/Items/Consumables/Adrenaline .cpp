#include "Adrenaline .h"
#include "../Characters/Character.h"
#include <QDebug>

Adrenaline::Adrenaline(QGraphicsItem *parent)
    : Consumable(parent, ":/adrenaline.png") {
    // 构造函数，使用肾上腺素图片
}

void Adrenaline::takeEffect(Character* character) {
    if (character && !character->isDead()) {
        // 启动肾上腺素效果：增加移动速度并开始持续回血
        character->startAdrenalineEffect(EFFECT_DURATION, SPEED_MULTIPLIER, HEAL_PER_FRAME);
        qDebug() << "Adrenaline used: speed boost and regeneration started";
    }
}