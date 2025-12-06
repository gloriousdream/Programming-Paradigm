#include "ArrowTower.h"
USING_NS_CC;

bool ArrowTower::init()
{
    if (!Building::init()) return false;

    setTexture("ArrowTower.png");
    level = 1;

    maxHP = 100;
    currentHP = maxHP;
    updateHPBar();

    return true;
}

void ArrowTower::upgrade()
{
    level++;
    if (level == 2) setTexture("ArrowTower2.png");
    else if (level == 3) setTexture("ArrowTower2.png");
}
