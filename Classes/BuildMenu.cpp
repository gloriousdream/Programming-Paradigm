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

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 半透明背景
    auto bg = LayerColor::create(Color4B(0, 0, 0, 150));
    this->addChild(bg);

    // 建筑类型与资源消耗
    struct BuildingInfo {
        std::string img;
        int type;
        int goldCost;
        int waterCost;
    };

    std::vector<BuildingInfo> buildings = {
        {"MilitaryCamp.png", 1, 50, 30},
        {"WaterCollection.png", 2, 40, 20},
        {"ArrowTower.png", 3, 70, 40},
        {"TownHall.png", 4, 150, 100}
    };

    std::vector<MenuItemImage*> menuItems;

    for (auto& info : buildings)
    {
        // 创建按钮
        auto item = MenuItemImage::create(
            info.img,
            info.img,
            [=](Ref*) {
                if (onSelectBuilding) onSelectBuilding(info.type);
            }
        );

        // 创建显示金币和圣水的小标签
        std::string costText = "G:" + std::to_string(info.goldCost) +
            " W:" + std::to_string(info.waterCost);
        auto label = Label::createWithSystemFont(costText, "Arial", 16);
        label->setColor(Color3B::YELLOW);
        label->setPosition(Vec2(item->getContentSize().width / 2, -20));
        item->addChild(label);

        menuItems.push_back(item);
    }

    // 创建 Menu 并居中
    auto menu = Menu::create(menuItems[0], menuItems[1], menuItems[2], menuItems[3], nullptr);
    menu->alignItemsHorizontallyWithPadding(50);
    menu->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + 150)); // 底部偏上
    this->addChild(menu, 10);

    return true;
}
