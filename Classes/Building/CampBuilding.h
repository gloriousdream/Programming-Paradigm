#ifndef __CAMP_BUILDING_H__
#define __CAMP_BUILDING_H__

#include "BaseBuilding.h"

class CampBuilding : public BaseBuilding
{
public:
    static CampBuilding* create();

    virtual void upgrade() override;
    virtual void updateTexture() override;
};

#endif
