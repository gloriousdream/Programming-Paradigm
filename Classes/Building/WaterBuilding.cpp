#include "WaterBuilding.h"
USING_NS_CC;

WaterBuilding* WaterBuilding::create()
{
    WaterBuilding* p = new WaterBuilding();
    if (p && p->initWithFile("WaterCollection.png"))
    {
        p->initWithType(2);
        p->autorelease();
        return p;
    }
    delete p;
    return nullptr;
}

void WaterBuilding::upgrade()
{
    _level++;
    CCLOG("Ë®¾®Éý¼¶µ½£º%d", _level);
    updateTexture();
}

void WaterBuilding::updateTexture()
{
    if (_level == 1)      setTexture("WaterCollection.png");
    else if (_level == 2) setTexture("WaterCollection.png");
    else if (_level == 3) setTexture("WaterCollection.png");
}
