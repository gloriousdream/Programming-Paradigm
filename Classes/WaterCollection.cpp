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

    return true;
}

void WaterCollection::upgrade()
{
    level++;
    if (level == 2) setTexture("WaterCollection.png");
    else if (level == 3) setTexture("WaterCollection.png");
}