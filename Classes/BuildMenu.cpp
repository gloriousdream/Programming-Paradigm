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

    // 1. 半透明背景
    auto bg = LayerColor::create(Color4B(0, 0, 0, 150));
    this->addChild(bg);

    // 2. 吞噬点击事件，防止点到底下地图
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [](Touch*, Event*) { return true; };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, bg);

    // 3. 定义建筑列表 
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
        {"TownHall.png", 4, 150, 100},
        {"CoinCollection.png", 5, 20, 80}
    };

    Vector<MenuItem*> menuItems;

    for (const auto& info : buildings)
    {
        auto item = MenuItemImage::create(
            info.img,
            info.img,
            [=](Ref*) {
                // 发送通知
                if (onSelectBuilding) {
                    onSelectBuilding(info.type);
                }
            }
        );

        // 显示价格
        std::string costText = "G:" + std::to_string(info.goldCost) + " W:" + std::to_string(info.waterCost);
        auto label = Label::createWithSystemFont(costText, "Arial", 16);
        label->setColor(Color3B::YELLOW);
        label->setPosition(Vec2(item->getContentSize().width / 2, -10));
        item->addChild(label);

        menuItems.pushBack(item);
    }

    // 4. 创建菜单并排列
    // 使用 createWithArray 支持动态数量
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    menu->alignItemsHorizontallyWithPadding(20);
    this->addChild(menu);

    return true;
}