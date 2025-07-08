// src/Items/Characters/Link.cpp

#include "Link.h"
#include "../HeadEquipments/CapOfTheHero.h"
#include "../Armors/OldShirt.h"
#include "../LegEquipments/WellWornTrousers.h"

// [MODIFIED] 将 playerID 传递给 Character 基类
Link::Link(int playerID, QGraphicsItem *parent) : Character(playerID, parent) {
    // 你的装备初始化代码非常棒，保持原样
    headEquipment = new CapOfTheHero(this);
    legEquipment = new WellWornTrousers(this);
    armor = new OldShirt(this);
    headEquipment->mountToParent();
    legEquipment->mountToParent();
    armor->mountToParent();
}
