#include "Medkit.h"
#include "../Characters/Character.h"
#include <QDebug>

Medkit::Medkit(QGraphicsItem *parent)
    : Consumable(parent, ":/medkit.svg") {
    // 构造函数，使用医疗包图片
}

void Medkit::takeEffect(Character* character) {
    if (character && !character->isDead()) {
        character->setHealth(character->getMaxHealth());
        qDebug() << "Medkit used: fully healed to" << character->getMaxHealth() << "HP";
    }
}