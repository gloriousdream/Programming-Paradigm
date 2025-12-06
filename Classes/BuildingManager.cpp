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
static const int BUILD = 128;

static Vec2 snapToGrid(Vec2 pos)
{
    int col = pos.x / TILE;
    int row = pos.y / TILE;
    return Vec2(col * TILE + TILE, row * TILE + TILE);
}

bool BuildingManager::canPlaceBuilding(Vec2 pos)
{
    Vec2 p = snapToGrid(pos);
    int col = (p.x - TILE) / TILE;
    int row = (p.y - TILE) / TILE;

    for (int dx = 0; dx < 2; dx++)
        for (int dy = 0; dy < 2; dy++)
        {
            int c = col + dx;
            int r = row + dy;
            if (c < 0 || c >= 32 || r < 0 || r >= 24) return false;
            if (grid[c][r]) return false;
        }
    return true;
}

void BuildingManager::occupyGrid(Vec2 pos)
{
    Vec2 p = snapToGrid(pos);
    int col = (p.x - TILE) / TILE;
    int row = (p.y - TILE) / TILE;

    for (int dx = 0; dx < 2; dx++)
        for (int dy = 0; dy < 2; dy++)
            grid[col + dx][row + dy] = true;
}

Building* BuildingManager::createBuilding(int type, Vec2 pos)
{
    if (!canPlaceBuilding(pos))
    {
        CCLOG("格子被占用");
        return nullptr;
    }

    Building* building = nullptr;
    switch (type)
    {
        case 1: building = MilitaryCamp::create(); break;
        case 2: building = WaterCollection::create(); break;
        case 3: building = ArrowTower::create(); break;
        default: return nullptr;
    }

    Vec2 p = snapToGrid(pos);
    building->setPosition(p);
    building->setContentSize(Size(BUILD, BUILD));
    occupyGrid(pos);

    return building;
}
