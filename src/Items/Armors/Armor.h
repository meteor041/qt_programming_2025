//
// Created by gerw on 8/20/24.
//

#ifndef QT_PROGRAMMING_2024_ARMOR_H
#define QT_PROGRAMMING_2024_ARMOR_H

#include "../Item.h"
#include "../Mountable.h"
#include "../Weapon/Weapon.h" // 【新增】包含Weapon.h以识别武器类型

class Armor : public Item, public Mountable {
public:
    explicit Armor(QGraphicsItem *parent, const QString &pixmapPath);

    void mountToParent() override;

    void unmount() override;
    // --- 【新增】核心功能：处理伤害的纯虚函数 ---
    // 这个函数将由具体的护甲类（如锁子甲、防弹衣）来实现
    // 参数：
    //   - incomingDamage: 传入的原始伤害值
    //   - sourceWeapon:   造成伤害的武器指针，用于判断武器类型
    // 返回值：
    //   - 经过护甲减免后，角色应承受的最终伤害
    virtual int processDamage(int incomingDamage, Weapon* sourceWeapon) = 0;

    // 【新增】获取护甲名称的纯虚函数，用于UI显示
    [[nodiscard]] virtual QString getName() const = 0;

    // --- 【新增】耐久度相关公共函数 ---
    // 获取当前耐久度
    [[nodiscard]] int getDurability() const;

    // 获取最大耐久度
    [[nodiscard]] int getMaxDurability() const;

    // 检查护甲是否已损坏
    [[nodiscard]] bool isBroken() const;

protected:
    // --- 【新增】耐久度相关保护成员变量 ---
    int m_durability{100};      // 当前耐久度
    int m_maxDurability{100};   // 最大耐久度
    bool m_isBroken{false};     // 护甲是否已损坏
};


#endif //QT_PROGRAMMING_2024_ARMOR_H
