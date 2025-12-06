#ifndef __TOWER_BUILDING_H__
#define __TOWER_BUILDING_H__

#include "BaseBuilding.h"

class TowerBuilding : public BaseBuilding
{
public:
    static TowerBuilding* create();

    virtual void upgrade() override;
    virtual void updateTexture() override;
};

#endif
