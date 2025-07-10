// src/Items/Maps/platform/IcePlatform.cpp

#include "IcePlatform.h"
#include "../../Characters/Character.h" // 确保包含Character头文件

// ... 构造函数 ...
IcePlatform::IcePlatform(QGraphicsItem *parent)
    : Platform(":/platform3.png", parent) // 使用在.qrc中定义的资源路径
{
    // 这里是构造函数的函数体，可以留空，
    // 也可以进行该平台的特有初始化（如果需要的话）。
}

void IcePlatform::onCharacterEnter(Character* character) {
    Platform::onCharacterEnter(character); // 好习惯
    if (character) {
        qDebug() << "Character entered Ice Platform. Speeding up!";
        // 【解锁】调用Character的方法
        character->setSpeedMultiplier(1.5);
    }
}

void IcePlatform::onCharacterLeave(Character* character) {
    Platform::onCharacterLeave(character);
    if (character) {
        qDebug() << "Character left Ice Platform. Restoring speed.";
        // 【解锁】调用Character的方法
        character->setSpeedMultiplier(1.0);
    }
}
