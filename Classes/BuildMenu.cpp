#include "BuildMenu.h"

USING_NS_CC;

BuildMenu* BuildMenu::createMenu()
{
    return BuildMenu::create();
}

bool BuildMenu::init()
{
    if (!Layer::init()) return false;

    // 半透明背景
    auto bg = LayerColor::create(Color4B(0, 0, 0, 150));
    this->addChild(bg);

    // 创建3个建筑icon
    auto goldMine = MenuItemImage::create(
        "icons/goldmine.png",
        "icons/goldmine.png",
        [=](Ref*) { if (onSelectBuilding) onSelectBuilding(1); }
    );

    auto water = MenuItemImage::create(
        "icons/water.png",
        "icons/water.png",
        [=](Ref*) { if (onSelectBuilding) onSelectBuilding(2); }
    );

    auto barrack = MenuItemImage::create(
        "icons/barrack.png",
        "icons/barrack.png",
        [=](Ref*) { if (onSelectBuilding) onSelectBuilding(3); }
    );

    auto menu = Menu::create(goldMine, water, barrack, nullptr);
    menu->alignItemsHorizontallyWithPadding(50);
    menu->setPosition(Vec2(1024, 200));
    this->addChild(menu);

    return true;
}
