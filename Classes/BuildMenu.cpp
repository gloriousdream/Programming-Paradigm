#include "BuildMenu.h"
#include "cocos2d.h"
USING_NS_CC;

BuildMenu* BuildMenu::createMenu()
{
    return BuildMenu::create();
}

bool BuildMenu::init()
{
    if (!Layer::init()) return false;

    auto bg = LayerColor::create(Color4B(0, 0, 0, 150));
    this->addChild(bg);

    auto military = MenuItemImage::create("MilitaryCamp.png", "MilitaryCamp.png", [=](Ref*) { if (onSelectBuilding) onSelectBuilding(1); });
    auto water = MenuItemImage::create("WaterCollection.png", "WaterCollection.png", [=](Ref*) { if (onSelectBuilding) onSelectBuilding(2); });
    auto arrow = MenuItemImage::create("ArrowTower.png", "ArrowTower.png", [=](Ref*) { if (onSelectBuilding) onSelectBuilding(3); });

    auto menu = Menu::create(military, water, arrow, nullptr);
    menu->alignItemsHorizontallyWithPadding(50);
    menu->setPosition(Vec2(1024, 200));
    this->addChild(menu);

    return true;
}
