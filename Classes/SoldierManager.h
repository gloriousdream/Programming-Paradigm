#pragma once
#include "cocos2d.h"
#include <vector>
#include "Soldier.h"
#include "Barbarian.h"
#include "Giant.h"
class SoldierManager
{
public:
    static SoldierManager* getInstance();

    Soldier* createSoldier(int type, cocos2d::Vec2 pos);

private:
    SoldierManager();

    // 32 × 24 的格子表//用于标记哪里有建筑物
    bool grid[30][16];
};