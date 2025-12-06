#include "cocos2d.h"
#include <vector>

class BuildingManager
{
public:
    static BuildingManager* getInstance();

    cocos2d::Sprite* createBuilding(int type, cocos2d::Vec2 pos);

    // 检测格子是否被占
    bool canPlaceBuilding(cocos2d::Vec2 pos);

    // 放置建筑后标记格子
    void occupyGrid(cocos2d::Vec2 pos);

private:
    BuildingManager();

    // 32 × 24 的格子表
    bool grid[32][24];
};