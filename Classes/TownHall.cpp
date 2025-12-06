#include "TownHall.h"
USING_NS_CC;

bool TownHall::init()
{
    if (!Building::init()) return false;

    setTexture("TownHall1.png");
    level = 1;

    maxHP = 500;
    currentHP = maxHP;
    updateHPBar();

    return true;
}

void TownHall::upgrade()
{
    level++;
    if (level == 2) setTexture("TownHall2.png");
    else if (level == 3) setTexture("TownHall3.png");
}