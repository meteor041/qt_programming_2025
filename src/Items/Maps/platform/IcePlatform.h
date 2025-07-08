#ifndef ICEPLATFORM_H
#define ICEPLATFORM_H

// IcePlatform.h

#include "Platform.h"

class IcePlatform : public Platform {
public:
    /**
     * @brief 构造一个冰场平台。
     * @param parent 父QGraphicsItem。
     */
    explicit IcePlatform(QGraphicsItem *parent = nullptr);

    /**
     * @brief 当角色进入冰场时被调用，使其加速。
     * @param character 进入平台的角色。
     */
    void onCharacterEnter(Character* character) override;

    /**
     * @brief 当角色离开冰场时被调用，使其恢复原速。
     * @param character 离开平台的角色。
     */
    void onCharacterLeave(Character* character) override;
};

#endif // ICEPLATFORM_H
