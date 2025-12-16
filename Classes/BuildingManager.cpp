#include "BuildingManager.h"

USING_NS_CC;

static BuildingManager* _instance = nullptr;
static const int TILE = 64;
BuildingManager* BuildingManager::getInstance()
{
    if (!_instance)
        _instance = new BuildingManager();
    return _instance;
}

BuildingManager::BuildingManager()
{
    // 1. 初始化所有格子为空
    memset(grid, 0, sizeof(grid));

    // 2.初始化保留区域 (占位)
    initReservedArea();
}

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
void BuildingManager::initReservedArea()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // --- 计算目标位置 (屏幕左侧 20%，高度 50%) ---
    float targetX = visibleSize.width * 0.2f;
    float targetY = visibleSize.height * 0.5f;

    // --- 转换为格子索引 ---
    // 我们希望这块区域是 2x2 的格子 (128x128)
    // 所以我们找一个左下角的格子索引
    reservedCol = (int)(targetX / TILE);
    reservedRow = (int)(targetY / TILE);

    // 修正边界防止越界
    if (reservedCol < 0) reservedCol = 0;
    if (reservedCol > 30) reservedCol = 30; // 32 - 2
    if (reservedRow < 0) reservedRow = 0;
    if (reservedRow > 22) reservedRow = 22; // 24 - 2

    // --- [关键] 标记格子为占用 ---
    // 2x2 区域
    grid[reservedCol][reservedRow] = true;         // 左下
    grid[reservedCol + 1][reservedRow] = true;     // 右下
    grid[reservedCol][reservedRow + 1] = true;     // 左上
    grid[reservedCol + 1][reservedRow + 1] = true; // 右上

    CCLOG("Reserved Area initialized at Grid[%d][%d]", reservedCol, reservedRow);
}
Rect BuildingManager::getSoldierSpawnArea()
{
    // 将格子索引转回世界坐标 (左下角原点)
    float x = reservedCol * TILE;
    float y = reservedRow * TILE;
    float w = TILE * 2; // 2格宽
    float h = TILE * 2; // 2格高

    return Rect(x, y, w, h);
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
        case 5: building = CoinCollection::create(); break;
        case 6: building = Cannon::create(); break;
        case 7: building = GoldStage::create(); break;
        case 8: building = ElixirTank::create(); break;
        default: return nullptr;
    }

    int size = getBuildingGridSize(type);
    Vec2 p = snapToGrid(pos, size);
    building->setPosition(p);
    if (type != 6)
    {
        building->setContentSize(Size(TILE * size, TILE * size));
    }

    occupyGrid(pos, type);

    return building;
}
