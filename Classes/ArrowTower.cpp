#include "ArrowTower.h"
USING_NS_CC;

bool ArrowTower::init()
{
    if (!Building::init()) return false;
    level = 1;
    setTexture("ArrowTower.png");
    return true;
}

void ArrowTower::upgrade()
{
    level++;
    if (level == 2) setTexture("ArrowTower2.png");
    else if (level == 3) setTexture("ArrowTower2.png");
}
