#include "TownHall.h"
USING_NS_CC;

bool TownHall::init()
{
    if (!Building::init()) return false;
    level = 1;
    setTexture("TownHall1.png");
    return true;
}

void TownHall::upgrade()
{
    level++;
    if (level == 2) setTexture("TownHall2.png");
    else if (level == 3) setTexture("TownHall3.png");
}