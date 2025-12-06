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
