#include "ArrowTower.h"
USING_NS_CC;

bool ArrowTower::init()
{
    if (!Building::init()) return false;

    // 初始贴图与属性
    setTexture("ArrowTower.png");
    level = 1;
    maxHP = 100;
    currentHP = maxHP;
    updateHPBar();

    // 建造和升级消耗（你同意的数值）
    buildCostGold = 120;
    buildCostHoly = 60;
    // 升级消耗：到2级 / 到3级（GameScene 会读取这些值）
    upgradeCostGold = 60;  // 这里表示下一次升级的当前数值（we will interpret in GameScene）
    upgradeCostHoly = 30;

    return true;
}

void ArrowTower::upgrade()
{
    level++;
    if (level == 2) {
        setTexture("ArrowTower2.png");
        maxHP = 140;
    }
    else if (level == 3) {
        setTexture("ArrowTower3.png");
        maxHP = 180;
    }
    // 每次升级恢复满血
    currentHP = maxHP;
    updateHPBar();

    // update upgradeCost to next level (optional): we keep static per-level costs in GameScene logic
}
