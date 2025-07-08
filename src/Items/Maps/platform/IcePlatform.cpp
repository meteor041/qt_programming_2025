// IcePlatform.cpp

#include "IcePlatform.h"
#include "../../Characters/Character.h" // 包含Character头文件以访问其方法

/**
 * @brief IcePlatform的构造函数。
 * 它调用基类Platform的构造函数，并传入冰块平台的贴图资源路径。
 */
IcePlatform::IcePlatform(QGraphicsItem *parent)
    : Platform(":/platform3.png", parent) // 使用在.qrc中定义的资源路径
{
    // 可以在这里进行该平台的特有初始化（如果需要的话）
}

/**
 * @brief 重写onCharacterEnter方法。
 * 当角色接触到这个平台时，将其移动速度倍率设置为1.5（或任何你想要的值）。
 */
void IcePlatform::onCharacterEnter(Character* character) {
    // 调用基类的方法（虽然是空的，但是个好习惯）
    Platform::onCharacterEnter(character);

    // 假设Character类有一个名为setSpeedMultiplier的方法
    if (character) {
    //还没写    character->setSpeedMultiplier(1.5); // 移动速度变为1.5倍
    }
}

/**
 * @brief 重写onCharacterLeave方法。
 * 当角色离开这个平台时，将其移动速度倍率恢复为1.0（正常速度）。
 */
void IcePlatform::onCharacterLeave(Character* character) {
    // 调用基类的方法
    Platform::onCharacterLeave(character);

    // 恢复正常速度
    if (character) {
    //还没写    character->setSpeedMultiplier(1.0);
    }
}
