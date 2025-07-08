#include "Platform.h"

class EarthPlatform : public Platform {
public:
    // 构造函数直接调用基类构造函数并传入土地贴图的路径
    explicit EarthPlatform(QGraphicsItem *parent = nullptr);
};
