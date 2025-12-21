#include "TownHall.h"
USING_NS_CC;

bool TownHall::init()
{
    if (!Building::init()) return false;

    setTexture("TownHall.png");
    level = 1;
    maxHP = 500;
    currentHP = maxHP;
    updateHPBar();

    buildCostGold = 200;
    buildCostHoly = 150;

    upgradeCostGold = 120;
    upgradeCostHoly = 100;

    return true;
}
std::string TownHall::getNextLevelTextureName()
{
    // 注意：当前 level 是 1，下一级就是 2
    int nextLv = level + 1;

    if (nextLv == 2) return "TownHall2.png";
    if (nextLv == 3) return "TownHall3.png";

    return ""; // 满级了或没有图
}
void TownHall::upgrade()
{
    level++;
    if (level == 2) {
        setTexture("TownHall2.png");
        maxHP = 650;
    }
    else if (level == 3) {
        setTexture("TownHall3.png");
        maxHP = 800;
    }
    currentHP = maxHP;
    updateHPBar();
}
