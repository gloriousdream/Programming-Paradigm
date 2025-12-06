#include "WaterCollection.h"
USING_NS_CC;

bool WaterCollection::init()
{
    if (!Building::init()) return false;
    level = 1;
    setTexture("WaterCollection.png");
    return true;
}

void WaterCollection::upgrade()
{
    level++;
    if (level == 2) setTexture("WaterCollection.png");
    else if (level == 3) setTexture("WaterCollection.png");
}