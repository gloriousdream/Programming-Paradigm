#pragma once
#include "cocos2d.h"
#include <vector>

class SoldierManager
{
public:
    static SoldierManager* getInstance();

    cocos2d::Sprite* createSoldier(int type, cocos2d::Vec2 pos);

private:
    SoldierManager();

    // 32 × 24 的格子表//用于标记哪里有建筑物
    bool grid[32][24];
};
