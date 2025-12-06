#include "BaseBuilding.h"
USING_NS_CC;

bool BaseBuilding::initWithType(int type)
{
    _type = type;
    _level = 1;

    // 每个建筑的名字格式统一
    char nameBuf[32];
    sprintf(nameBuf, "BUILDING_%d", type);
    this->setName(nameBuf);

    return true;
}
