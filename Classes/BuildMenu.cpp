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
    //先加载图集！

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("atlas.plist");

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 半透明背景
    auto bg = LayerColor::create(Color4B(0, 0, 0, 150));
    this->addChild(bg);

    // 吞噬点击
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [](Touch*, Event*) { return true; };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, bg);

    // 定义建筑列表 
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
        {"CoinCollection.png", 5, 20, 80},
        {"cannon_stand.png", 6, 200, 0},
        { "gold_stage_05.png", 7, 0, 0 },
        { "elixir_tank_05.png", 8, 0, 0 },
        { "boom1.png", 9, 150, 0 }
    };

    Vector<MenuItem*> menuItems;

    for (const auto& info : buildings)
    {
        MenuItem* item = nullptr;

        // 先去缓存里查，看看是不是 plist 里的图
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(info.img);

        if (frame) {
            // A. 如果是图集里的图 (cannon_stand.png)
            // 必须先创建 Sprite，再包装成 MenuItemSprite
            auto normalSprite = Sprite::createWithSpriteFrameName(info.img);
            auto selectedSprite = Sprite::createWithSpriteFrameName(info.img);
            selectedSprite->setColor(Color3B::GRAY); // 选中时变暗一点，给个反馈

            item = MenuItemSprite::create(
                normalSprite,
                selectedSprite,
                [=](Ref*) {
                    if (onSelectBuilding) onSelectBuilding(info.type);
                }
            );
        }
        else {
            // B. 如果是普通文件 (TownHall.png 等)
            // 直接用 MenuItemImage 读取文件
            item = MenuItemImage::create(
                info.img,
                info.img, // 选中图偷懒用同一张，或者你可以准备一张变暗的图
                [=](Ref*) {
                    if (onSelectBuilding) onSelectBuilding(info.type);
                }
            );
        }

        // 如果图片缺失，创建一个红叉占位，防止崩溃
        if (!item) {
            auto errSpr = Sprite::create();
            errSpr->setTextureRect(Rect(0, 0, 50, 50));
            errSpr->setColor(Color3B::RED);
            item = MenuItemSprite::create(errSpr, errSpr, nullptr);
        }

        // 显示价格 
        std::string costText = "G:" + std::to_string(info.goldCost) + " W:" + std::to_string(info.waterCost);
        auto label = Label::createWithSystemFont(costText, "Arial", 16);
        label->setColor(Color3B::YELLOW);
        // 注意：如果是 plist 图片，getContentSize 可能会变，这里取的是 item 的大小，是安全的
        label->setPosition(Vec2(item->getContentSize().width / 2, -10));
        item->addChild(label);

        menuItems.pushBack(item);
    }

    // 创建菜单并排列
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    menu->alignItemsHorizontallyWithPadding(20);
    this->addChild(menu);

    // 关闭按钮
    auto closeBtn = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", [=](Ref*)
        {
            this->removeFromParent();
        });

    auto sysMenu = Menu::create(closeBtn, nullptr);
    sysMenu->setPosition(visibleSize.width - 50, visibleSize.height - 50);
    this->addChild(sysMenu, 10);
    return true;
}