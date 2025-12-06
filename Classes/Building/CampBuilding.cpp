#include "CampBuilding.h"
USING_NS_CC;

CampBuilding* CampBuilding::create()
{
    CampBuilding* p = new CampBuilding();
    if (p && p->initWithFile("MilitaryCamp.png"))
    {
        p->initWithType(1);
        p->autorelease();
        return p;
    }
    delete p;
    return nullptr;
}

void CampBuilding::upgrade()
{
    _level++;
    CCLOG("军营升级到：%d", _level);
    updateTexture();
}

void CampBuilding::updateTexture()
{
    if (_level == 1)      setTexture("MilitaryCamp.png");
    else if (_level == 2) setTexture("MilitaryCamp.png");
    else if (_level == 3) setTexture("MilitaryCamp.png");
}
