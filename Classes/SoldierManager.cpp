#include "SoldierManager.h"
#include "cocos2d.h"
#include "Barbarian.h"
#include "Archer.h" 
#include "Boomer.h"
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
        case 1: soldier = Barbarian::create(); break;
        case 2: soldier = Giant::create(); break;
        case 3: soldier = Archer::create(); break;
        case 4: soldier = Bomber::create(); break; 
    }

    if (soldier)
    {
        soldier->setPosition(pos);
        soldier->setHomePosition(pos); // 保留作为默认值

        _soldiers.pushBack(soldier);
    }
    else
    {
        CCLOGERROR("Soldier type %d create failed!", type);
    }

    return soldier;
}

// 清理死兵的实现
void SoldierManager::cleanDeadSoldiers()
{
    // 倒序遍历，安全删除
    for (int i = _soldiers.size() - 1; i >= 0; i--)
    {
        auto s = _soldiers.at(i);
        // 如果士兵由于某种原因没有父节点（被移除了），或者血量<=0
        if (!s || !s->getParent() || s->getHP() <= 0)
        {
            _soldiers.erase(i);
        }
    }
}