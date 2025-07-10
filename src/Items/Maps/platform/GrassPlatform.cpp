// src/Items/Maps/platform/GrassPlatform.cpp

#include "GrassPlatform.h"
#include "../../Characters/Character.h" // 包含Character

GrassPlatform::GrassPlatform(QGraphicsItem *parent)
    : Platform(":/platform1.png", parent) {}

/**
 * @brief 角色停留在草地上时，每帧调用。
 * 我们在这里检查角色是否下蹲，并据此设置其隐身状态。
 */

void GrassPlatform::onCharacterStay(Character* character) {
    Platform::onCharacterStay(character);
    if (character) {
        // 如果角色在草地上并且正在下蹲，则进入隐身
        if (character->isCrouching()) {
            character->setInStealth(true);
        } else {
            // 如果在草地上但没有下蹲，则不隐身
            character->setInStealth(false);
        }
    }
}

/**
 * @brief 角色离开草地时调用。
 * 无论之前是什么状态，离开后都必须取消隐身。
 */
void GrassPlatform::onCharacterLeave(Character* character) {
    Platform::onCharacterLeave(character);
    if (character) {
        // 确保角色离开草地时，隐身效果被移除
        character->setInStealth(false);
    }
}
