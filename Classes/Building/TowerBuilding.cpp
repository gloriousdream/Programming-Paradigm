#include "TowerBuilding.h"
USING_NS_CC;

TowerBuilding* TowerBuilding::create()
{
    TowerBuilding* p = new TowerBuilding();
    if (p && p->initWithFile("ArrowTower.png"))
    {
        p->initWithType(3);
        p->autorelease();
        return p;
    }
    delete p;
    return nullptr;
}

void TowerBuilding::upgrade()
{
    _level++;
    CCLOG("¼ýËþÉý¼¶µ½£º%d", _level);
    updateTexture();
}

void TowerBuilding::updateTexture()
{
    if (_level == 1)      setTexture("ArrowTower.png");
    else if (_level == 2) setTexture("ArrowTower2.png");
    else if (_level == 3) setTexture("ArrowTower2.png");
}
