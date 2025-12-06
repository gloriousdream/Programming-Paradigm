#include "WaterCollection.h"
USING_NS_CC;

bool WaterCollection::init()
{
    if (!Building::init()) return false;

    setTexture("WaterCollection.png");
    level = 1;
    maxHP = 120;
    currentHP = maxHP;
    updateHPBar();

    buildCostGold = 80;
    buildCostHoly = 80;

    upgradeCostGold = 40;
    upgradeCostHoly = 40;

    return true;
}

void WaterCollection::upgrade()
{
    level++;
    if (level == 2) {
        setTexture("WaterCollection.png");
        maxHP = 150;
    }
    else if (level == 3) {
        setTexture("WaterCollection.png");
        maxHP = 180;
    }
    currentHP = maxHP;
    updateHPBar();
}
