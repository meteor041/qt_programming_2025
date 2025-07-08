// src/Items/Characters/Link.h
#ifndef QT_PROGRAMMING_2024_LINK_H
#define QT_PROGRAMMING_2024_LINK_H

#include "Character.h"

class Link : public Character {
public:
    // [MODIFIED] 构造函数现在需要 playerID
    explicit Link(int playerID, QGraphicsItem *parent = nullptr);
};

#endif //QT_PROGRAMMING_2024_LINK_H
