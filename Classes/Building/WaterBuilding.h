#ifndef __WATER_BUILDING_H__
#define __WATER_BUILDING_H__

#include "BaseBuilding.h"

class WaterBuilding : public BaseBuilding
{
public:
    static WaterBuilding* create();

    virtual void upgrade() override;
    virtual void updateTexture() override;
};

#endif
