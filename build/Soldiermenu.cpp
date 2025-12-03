#include "Soldiermenu.h"

USING_NS_CC;

Soldiermenu* Soldiermenu::createMenu()
{
    return Soldiermenu::create();
}

bool Soldiermenu::init()
{
    if (!Layer::init()) return false;

    // °ëÍ¸Ã÷±³¾°
    auto bg = LayerColor::create(Color4B(0, 0, 0, 150));
    this->addChild(bg);

    // ´´½¨¹­¼ıÊÖ£¬Ò°ÂùÈË£¬Õ¨µ¯ÈË£¬¾ŞÈË
    auto yemanren = MenuItemImage::create(
        "yemanren_select.png",
        "yemanren_select.png",
        [=](Ref*) { if (onSelectSoldier) onSelectSoldier(1); }
    );

    auto juren = MenuItemImage::create(
        "juren_select.png",
        "juren_select.png",
        [=](Ref*) { if (onSelectSoldier) onSelectSoldier(2); }
    );

    auto gongjianshou = MenuItemImage::create(
        "gongjianshou_select.png",
        "gongjianshou_select.png",
        [=](Ref*) { if (onSelectSoldier) onSelectSoldier(3); }
    );
    auto boomer = MenuItemImage::create(
        "boom_select.png",
        "boom_select.png",
        [=](Ref*) { if (onSelectSoldier) onSelectSoldier(4); }
    );
    auto menu = Menu::create(gongjianshou,yemanren,juren,boomer ,nullptr);
    menu->alignItemsHorizontallyWithPadding(50);
    menu->setPosition(Vec2(1024, 200));
    this->addChild(menu);

    return true;
}
