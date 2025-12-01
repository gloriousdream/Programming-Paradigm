#include "cocos2d.h"

class BuildingManager
{
public:
    static BuildingManager* getInstance();

    cocos2d::Sprite* createBuilding(int type, cocos2d::Vec2 pos);
};
