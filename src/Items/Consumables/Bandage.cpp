#include "Bandage.h"
#include "../Characters/Character.h"
#include <QDebug>

Bandage::Bandage(QGraphicsItem *parent)
    : Consumable(parent, ":/bandage.svg") {
    // 构造函数，使用绷带图片
}

void Bandage::takeEffect(Character* character) {
    if (character && !character->isDead()) {
        character->heal(5);
        qDebug() << "Bandage used: healed 5 HP";
    }
}