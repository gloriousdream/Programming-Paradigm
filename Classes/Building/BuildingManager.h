#ifndef __BUILDING_MANAGER_H__
#define __BUILDING_MANAGER_H__

#include "cocos2d.h"
#include "Building/Building.h"
#include "Building/MilitaryCamp.h"
#include "Building/WaterCollection.h"
#include "Building/ArrowTower.h"
#include "Building/TownHall.h"
#include "Building/CoinCollection.h"
#include "Building/Cannon.h"
#include "Building/GoldStage.h"
#include "Building/ElixirTank.h"
#include "Building/Boom.h"
class BuildingManager
{
public:
    static BuildingManager* getInstance();

    // createBuilding 接受建筑类型和位置
    Building* createBuilding(int type, cocos2d::Vec2 pos);
    cocos2d::Rect getSoldierSpawnArea();
    //  获取士兵生成的保留区域 (Rect)
    bool canPlaceBuilding(cocos2d::Vec2 pos, int type);
    void occupyGrid(cocos2d::Vec2 pos, int type);

    // 重置
    void reset();
private:
    BuildingManager();
    bool grid[30][16];
    // 初始化士兵保留区
    void initReservedArea();
    int getBuildingGridSize(int type);
    // 存储保留区的格子索引，方便生成 Rect
    int reservedCol;
    int reservedRow;
};

#endif
