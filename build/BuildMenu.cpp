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

    // 创建3个建筑
    auto goldMine = MenuItemImage::create(
        "MilitaryCamp.png",
        "MilitaryCamp.png",
        [=](Ref*) { if (onSelectBuilding) onSelectBuilding(1); }
    );

    auto water = MenuItemImage::create(
        "WaterCollection.png",
        "WaterCollection.png",
        [=](Ref*) { if (onSelectBuilding) onSelectBuilding(2); }
    );

    auto barrack = MenuItemImage::create(
        "ArrowTower.png",
        "ArrowTower.png",
        [=](Ref*) { if (onSelectBuilding) onSelectBuilding(3); }
    );

    auto menu = Menu::create(goldMine, water, barrack, nullptr);
    menu->alignItemsHorizontallyWithPadding(50);
    menu->setPosition(Vec2(1024, 200));
    this->addChild(menu);

    return true;
}
