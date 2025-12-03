#ifndef __SOLDIER_MENU_H__
#define __SOLDIER_MENU_H__
#include "cocos2d.h"

class Soldiermenu : public cocos2d::Layer
{
public:
    static Soldiermenu* createMenu();

    std::function<void(int)> onSelectSoldier;

    virtual bool init();
    CREATE_FUNC(Soldiermenu);

private:
    void addBuildingIcon(const std::string& img, int type, float x, float y);
};
#endif

