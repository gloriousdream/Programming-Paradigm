#ifndef __BUILDING_MANAGER_H__
#define __BUILDING_MANAGER_H__

#include "cocos2d.h"
#include "Building.h"
#include "MilitaryCamp.h"
#include "WaterCollection.h"
#include "ArrowTower.h"
#include "TownHall.h"

class BuildingManager
{
public:
    static BuildingManager* getInstance();
    Building* createBuilding(int type, cocos2d::Vec2 pos);

    bool canPlaceBuilding(cocos2d::Vec2 pos, int type);
    void occupyGrid(cocos2d::Vec2 pos, int type);

private:
    BuildingManager();
    bool grid[32][24];

    int getBuildingGridSize(int type);
};

#endif
