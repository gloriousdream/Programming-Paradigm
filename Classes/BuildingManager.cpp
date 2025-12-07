#include "BuildingManager.h"

USING_NS_CC;

static BuildingManager* _instance = nullptr;

BuildingManager* BuildingManager::getInstance()
{
    if (!_instance)
        _instance = new BuildingManager();
    return _instance;
}

BuildingManager::BuildingManager()
{
    memset(grid, 0, sizeof(grid));
}

static const int TILE = 64;

// 返回建筑占用格子大小
int BuildingManager::getBuildingGridSize(int type)
{
    switch (type)
    {
        case 4: return 3; // TownHall 3x3
        default: return 2; // 其他建筑 2x2
    }
}

// 吸附到网格中心
static Vec2 snapToGrid(Vec2 pos, int gridSize)
{
    int col = pos.x / TILE;
    int row = pos.y / TILE;

    float snappedX = col * TILE + TILE * (gridSize / 2.0f);
    float snappedY = row * TILE + TILE * (gridSize / 2.0f);

    return Vec2(snappedX, snappedY);
}

// 是否可放置
bool BuildingManager::canPlaceBuilding(Vec2 pos, int type)
{
    int size = getBuildingGridSize(type);
    Vec2 p = snapToGrid(pos, size);

    int col = (p.x - TILE * (size / 2)) / TILE;
    int row = (p.y - TILE * (size / 2)) / TILE;

    for (int dx = 0; dx < size; dx++)
        for (int dy = 0; dy < size; dy++)
        {
            int c = col + dx;
            int r = row + dy;
            if (c < 0 || c >= 32 || r < 0 || r >= 24) return false;
            if (grid[c][r]) return false;
        }
    return true;
}

// 占用格子
void BuildingManager::occupyGrid(Vec2 pos, int type)
{
    int size = getBuildingGridSize(type);
    Vec2 p = snapToGrid(pos, size);

    int col = (p.x - TILE * (size / 2)) / TILE;
    int row = (p.y - TILE * (size / 2)) / TILE;

    for (int dx = 0; dx < size; dx++)
        for (int dy = 0; dy < size; dy++)
            grid[col + dx][row + dy] = true;
}

// 创建建筑（带类型和位置参数）
Building* BuildingManager::createBuilding(int type, Vec2 pos)
{
    if (!canPlaceBuilding(pos, type))
    {
        CCLOG("建筑放置失败：格子被占用");
        return nullptr;
    }

    Building* building = nullptr;
    switch (type)
    {
        case 1: building = MilitaryCamp::create(); break;
        case 2: building = WaterCollection::create(); break;
        case 3: building = ArrowTower::create(); break;
        case 4: building = TownHall::create(); break;
        default: return nullptr;
    }

    int size = getBuildingGridSize(type);
    Vec2 p = snapToGrid(pos, size);
    building->setPosition(p);
    building->setContentSize(Size(TILE * size, TILE * size));

    occupyGrid(pos, type);

    return building;
}
