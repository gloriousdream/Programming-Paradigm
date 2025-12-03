#include "SoldierManager.h"
#include "cocos2d.h"

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
static const int Soldier = 64;   // 士兵 64*64

// 将坐标吸附到格子(1*1格子的中心)
static Vec2 snapToGrid(Vec2 pos)
{
    int col = pos.x / TILE;
    int row = pos.y / TILE;

    float snappedX = col * TILE + 1/2*TILE;
    float snappedY = row * TILE + 1/2*TILE;

    return Vec2(snappedX, snappedY);
}
Sprite* SoldierManager::createSoldier(int type, Vec2 pos)
{
    Sprite* soldier = nullptr;

    switch (type)
    {
        case 1: soldier = Sprite::create("yemanren_select.png"); break;
        case 2: soldier = Sprite::create("juren_select.png"); break;
        case 3: soldier = Sprite::create("gongjianshou_select.png"); break;
        case 4: soldier = Sprite::create("boom_select.png"); break;
    }

    if (!soldier)
    {
        CCLOG("? 建筑图片没找到");
        return nullptr;
    }

    Vec2 p = snapToGrid(pos);
    soldier->setPosition(p);
    // 建筑放下成功后标记格子
    return soldier;
}
