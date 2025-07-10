// src/Items/Maps/platform/GrassPlatform.h

#include "Platform.h"

class GrassPlatform : public Platform {
public:
    explicit GrassPlatform(QGraphicsItem *parent = nullptr);

    // 草地逻辑比较特殊，在停留时根据角色是否下蹲来判断，所以需要重写 onCharacterStay
    // 离开时要确保取消隐身，所以也要重写 onCharacterLeave
    void onCharacterStay(Character* character) override;
    void onCharacterLeave(Character* character) override;
};
