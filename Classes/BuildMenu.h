#ifndef __BUILD_MENU_H__
#define __BUILD_MENU_H__

#include "cocos2d.h"
class BuildMenu : public cocos2d::Layer
{
public:
    static BuildMenu* createMenu();
    std::function<void(int)> onSelectBuilding;

    virtual bool init();
    CREATE_FUNC(BuildMenu);
private:
    void addBuildingIcon(const std::string& img, int type, float x, float y);
};
#endif