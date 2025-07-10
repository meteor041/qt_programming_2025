#include "Consumable.h"

Consumable::Consumable(QGraphicsItem *parent, const QString &pixmapPath)
    : Item(parent, pixmapPath) {
    // 基类构造函数，设置为可拾取状态
    Mountable::unmount();
}