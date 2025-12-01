#include "BuildingManager.h"
#include "cocos2d.h"

USING_NS_CC;

static BuildingManager* _instance = nullptr;

BuildingManager* BuildingManager::getInstance()
{
    if (!_instance)
        _instance = new BuildingManager();
    return _instance;
}

Sprite* BuildingManager::createBuilding(int type, Vec2 pos)
{
    Sprite* building = nullptr;

    switch (type)
    {
        case 1: building = Sprite::create("buildings/goldmine.png"); break;
        case 2: building = Sprite::create("buildings/water.png"); break;
        case 3: building = Sprite::create("buildings/barrack.png"); break;
    }

    if (building)
    {
        building->setPosition(pos);
    }
    return building;
}
