#include "BuildingManager.h"
#include "cocos2d.h"

USING_NS_CC;

static BuildingManager* _instance = nullptr;

BuildingManager::BuildingManager()
{
    // 初始化所有格子为空
    memset(grid, 0, sizeof(grid));
}

BuildingManager* BuildingManager::getInstance()
{
    if (!_instance)
        _instance = new BuildingManager();
    return _instance;
}

static const int TILE = 64;     // 格子 64×64
static const int BUILD = 128;   // 建筑 128×128

// 将坐标吸附到格子(2×2建筑的中心)
static Vec2 snapToGrid(Vec2 pos)
{
    int col = pos.x / TILE;
    int row = pos.y / TILE;

    float snappedX = col * TILE + TILE;
    float snappedY = row * TILE + TILE;

    return Vec2(snappedX, snappedY);
}

// =============================
// 检查是否可以放建筑（2×2）
// =============================
bool BuildingManager::canPlaceBuilding(Vec2 pos)
{
    Vec2 p = snapToGrid(pos);

    // 获取建筑左下角所在的格子
    int col = (p.x - TILE) / TILE;
    int row = (p.y - TILE) / TILE;

    // 2×2 格子范围
    for (int dx = 0; dx < 2; dx++)
    {
        for (int dy = 0; dy < 2; dy++)
        {
            int c = col + dx;
            int r = row + dy;

            // 越界直接不能放
            if (c < 0 || c >= 32 || r < 0 || r >= 24)
                return false;

            if (grid[c][r])  // 已被占用
                return false;
        }
    }
    return true;
}

// 标记格子为占用
void BuildingManager::occupyGrid(Vec2 pos)
{
    Vec2 p = snapToGrid(pos);

    int col = (p.x - TILE) / TILE;
    int row = (p.y - TILE) / TILE;

    for (int dx = 0; dx < 2; dx++)
    {
        for (int dy = 0; dy < 2; dy++)
        {
            int c = col + dx;
            int r = row + dy;
            grid[c][r] = true;
        }
    }
}

Sprite* BuildingManager::createBuilding(int type, Vec2 pos)
{
    // 先检查格子是否已经有人占了
    if (!canPlaceBuilding(pos))
    {
        CCLOG("❌ 建筑放置失败：格子被占用!");
        return nullptr;
    }

    Sprite* building = nullptr;

    switch (type)
    {
        case 1: building = Sprite::create("MilitaryCamp.png"); break;
        case 2: building = Sprite::create("WaterCollection.png"); break;
        case 3: building = Sprite::create("ArrowTower.png"); break;
    }

    if (!building)
    {
        CCLOG("❌ 建筑图片没找到");
        return nullptr;
    }

    Vec2 p = snapToGrid(pos);
    building->setPosition(p);
    building->setContentSize(Size(BUILD, BUILD));

    // 建筑放下成功后标记格子
    occupyGrid(pos);

    return building;
}
