//
// Created by gerw on 8/20/24.
//

#include "Armor.h"
#include <QGraphicsScene>

// 【核心修正】构造函数初始化列表现在只调用 Item 的构造函数
// 因为 Mountable 没有可调用的构造函数，它会被默认构造
Armor::Armor(QGraphicsItem *parent, const QString &pixmapPath)
    : Item(parent, pixmapPath) // 调用 Item 的构造函数
{
    // Armor 的构造函数体
}

// --- 【新增】实现耐久度相关的 get 方法 ---
int Armor::getDurability() const {
    return m_durability;
}

int Armor::getMaxDurability() const {
    return m_maxDurability;
}

bool Armor::isBroken() const {
    return m_isBroken;
}

// --- 【新增】实现标记删除相关方法 ---
bool Armor::isMarkedForDeletion() const {
    return m_markedForDeletion;
}

// --- 【修改后的最终版本】 ---
// 这个方法会被所有护甲子类继承
void Armor::mountToParent() {
    // 1. 【重要】调用基类方法，以设置 mounted = true
    Mountable::mountToParent();

    // 2. 装备时，隐藏护甲自身的图片。
    if (pixmapItem) {
        pixmapItem->setVisible(false);
    }
}

// 这个方法会被所有护甲子类继承
void Armor::unmount() {
    // 1. 【重要】调用基类方法，以设置 mounted = false
    Mountable::unmount();

    // 2. 被丢弃时，使其图片可见。
    if (pixmapItem) {
        pixmapItem->setVisible(true);
    }
}

void Armor::markForDeletion() {
    if (m_markedForDeletion) return; // 防止重复标记

    m_markedForDeletion = true;
    qDebug() << "Armor marked for deletion.";

    // 【！！！关键修改！！！】
    // 模仿 ShotPut 的逻辑：将自己从父项 (Character) 上脱离。
    // 这样它就变成了场景中的一个顶层 Item，
    // BattleScene 的 items() 循环才能遍历到它并进行清理。
    if (this->parentItem()) {
        this->setParentItem(nullptr);
    }

    // 也可以选择立即隐藏它，虽然它已经是坏的了
    this->hide();
}
