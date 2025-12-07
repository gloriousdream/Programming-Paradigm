#include "SoldierManager.h"
#include "cocos2d.h"
#include "Barbarian.h"
USING_NS_CC;

static SoldierManager* _instance = nullptr;

SoldierManager::SoldierManager()
{
    //标记被占用的格子
    
    // 初始化所有格子为空
    memset(grid, 0, sizeof(grid));
}

SoldierManager* SoldierManager::getInstance()
{
    if (!_instance)
        _instance = new SoldierManager();
    return _instance;
}

static const int TILE = 64;     // 格子 64×64
static const int shibing = 64;   // 士兵 64*64

// 将坐标吸附到格子(1*1格子的中心)
static Vec2 snapToGrid(Vec2 pos)
{
    int col = pos.x / TILE;
    int row = pos.y / TILE;

    float snappedX = col * TILE + 1/2*TILE;
    float snappedY = row * TILE + 1/2*TILE;

    return Vec2(snappedX, snappedY);
}
Soldier* SoldierManager::createSoldier(int type, Vec2 pos)
{
    Soldier* soldier = nullptr;

    switch (type)
    {
        case 1: // 野蛮人
            soldier = Barbarian::create();
            break;
        case 2:
            // soldier = Giant::create(); 
            break;
        case 3:
            // soldier = Archer::create(); 
            break;
        case 4:
            // soldier = Bomber::create(); 
            break;
    }

    if (soldier)
    {
        soldier->setPosition(pos);
        // 设置出生点作为巡逻中心
        soldier->setHomePosition(pos);
        // 立即开始动作
        soldier->actionWalk();
    }
    else
    {
        CCLOGERROR("Soldier type %d not implemented yet!", type);
    }

    return soldier;
}