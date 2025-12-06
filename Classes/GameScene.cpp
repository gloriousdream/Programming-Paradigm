#include "GameScene.h"
#include "BuildMenu.h"
#include "BuildingManager.h"
#include "Soldiermenu.h"
#include "SoldierManager.h"
#include "Building.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

bool GameScene::init()
{
    if (!Scene::init()) return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 背景
    auto bg = Sprite::create("GrassBackground.png");
    bg->setAnchorPoint(Vec2::ZERO);
    bg->setPosition(origin);
    this->addChild(bg, 0);

    // 建造、士兵按钮
    auto buildBtn = MenuItemImage::create("Building.png", "Building.png", CC_CALLBACK_0(GameScene::onBuildButtonPressed, this));
    auto soldierBtn = MenuItemImage::create("Soldier.png", "Soldier.png", CC_CALLBACK_0(GameScene::onSoldierpushed, this));
    auto menu = Menu::create(buildBtn, soldierBtn, nullptr);
    menu->setPosition(origin.x + visibleSize.width - 120, origin.y + visibleSize.height / 2);
    menu->alignItemsVerticallyWithPadding(50);
    this->addChild(menu, 10);

    // 资源显示
    auto goldSprite = Sprite::create("GoldCoin.png");
    goldSprite->setPosition(Vec2(origin.x + visibleSize.width - 100, origin.y + visibleSize.height - 50));
    this->addChild(goldSprite, 10);
    goldLabel = Label::createWithTTF(std::to_string(gold), "fonts/Marker Felt.ttf", 24);
    goldLabel->setAnchorPoint(Vec2(0, 0.5f));
    goldLabel->setPosition(goldSprite->getPosition() + Vec2(20, 0));
    this->addChild(goldLabel, 10);

    auto waterSprite = Sprite::create("HolyWater.png");
    waterSprite->setPosition(Vec2(origin.x + visibleSize.width - 100, origin.y + visibleSize.height - 90));
    this->addChild(waterSprite, 10);
    waterLabel = Label::createWithTTF(std::to_string(holyWater), "fonts/Marker Felt.ttf", 24);
    waterLabel->setAnchorPoint(Vec2(0, 0.5f));
    waterLabel->setPosition(waterSprite->getPosition() + Vec2(20, 0));
    this->addChild(waterLabel, 10);

    // 地图点击事件
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [=](Touch* t, Event* e) {
        if (placeModebuild || placeModesoldier)
        {
            onMapClicked(t->getLocation());
            return true;
        }
        return false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // 建筑点击事件
    auto buildingListener = EventListenerCustom::create("BUILDING_CLICKED", [=](EventCustom* event) {
        Sprite* building = (Sprite*)event->getUserData();
        onBuildingClicked(building);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(buildingListener, this);

    return true;
}

// 刷新右上角资源显示
void GameScene::updateResourceDisplay()
{
    goldLabel->setString(std::to_string(gold));
    waterLabel->setString(std::to_string(holyWater));
}

// 点击建筑
void GameScene::onBuildingClicked(Sprite* building)
{
    // 如果当前显示的升级菜单正是这个建筑，隐藏
    if (currentBuildingMenu == building)
    {
        auto menu = this->getChildByName("UPGRADE_MENU");
        if (menu) menu->removeFromParent();
        currentBuildingMenu = nullptr;
    }
    else
    {
        currentBuildingMenu = building;
        showUpgradeButton(building);
    }
}

// 显示升级按钮和资源消耗
void GameScene::showUpgradeButton(Sprite* building)
{
    // 删除已有菜单
    if (auto existingMenu = this->getChildByName("UPGRADE_MENU"))
        existingMenu->removeFromParent();

    Vec2 pos = building->getPosition() + Vec2(0, 100);
    auto upgradeNode = Node::create();
    upgradeNode->setName("UPGRADE_MENU");
    upgradeNode->setPosition(pos);
    this->addChild(upgradeNode, 999);

    // 升级按钮
    auto btn = MenuItemImage::create(
        "UpgradeButton.png",
        "UpgradeButton.png",
        [=](Ref*) {
            Building* b = dynamic_cast<Building*>(building);
            if (b)
            {
                // 升级消耗（示例，可根据建筑类型和等级调整）
                int requiredGold = 50;
                int requiredHolyWater = 30;

                if (gold >= requiredGold && holyWater >= requiredHolyWater)
                {
                    gold -= requiredGold;
                    holyWater -= requiredHolyWater;
                    updateResourceDisplay();
                    b->upgrade();
                }
                else
                {
                    CCLOG("资源不足，无法升级");
                }
            }
            upgradeNode->removeFromParent();
            currentBuildingMenu = nullptr;
        }
    );
    auto menu = Menu::create(btn, nullptr);
    menu->setPosition(Vec2::ZERO);
    upgradeNode->addChild(menu);

    // 显示升级资源消耗
    int requiredGold = 50;
    int requiredHolyWater = 30;
    auto goldLabelNode = Label::createWithTTF("G:" + std::to_string(requiredGold), "fonts/Marker Felt.ttf", 20);
    goldLabelNode->setAnchorPoint(Vec2(0, 0.5f));
    goldLabelNode->setPosition(Vec2(btn->getContentSize().width / 2 + 10, 0));
    upgradeNode->addChild(goldLabelNode);

    auto waterLabelNode = Label::createWithTTF("H:" + std::to_string(requiredHolyWater), "fonts/Marker Felt.ttf", 20);
    waterLabelNode->setAnchorPoint(Vec2(0, 0.5f));
    waterLabelNode->setPosition(Vec2(btn->getContentSize().width / 2 + 10, -25));
    upgradeNode->addChild(waterLabelNode);
}

// 建造按钮
void GameScene::onBuildButtonPressed()
{
    auto menu = BuildMenu::createMenu();
    this->addChild(menu, 100);

    menu->onSelectBuilding = [menu, this](int type) {
        enablePlaceMode(type, menu);
        menu->removeFromParent();
        };
}

// 士兵按钮
void GameScene::onSoldierpushed()
{
    auto menu = Soldiermenu::createMenu();
    this->addChild(menu, 100);

    menu->onSelectSoldier = [menu, this](int type) {
        enablePlaceMode(type, menu);
        menu->removeFromParent();
        };
}

// 启用建造/士兵放置模式
template<typename T>
void GameScene::enablePlaceMode(int type, T menu)
{
    if (typeid(*menu) == typeid(BuildMenu)) placeModebuild = true;
    if (typeid(*menu) == typeid(Soldiermenu)) placeModesoldier = true;
    selectedType = type;
}

// 点击地图放置建筑/士兵
void GameScene::onMapClicked(Vec2 pos)
{
    if (!(placeModebuild || placeModesoldier)) return;

    int gx = pos.x / 64;
    int gy = pos.y / 64;
    Vec2 snapPos(gx * 64 + 32, gy * 64 + 32);

    if (placeModebuild)
    {
        // 建造资源消耗
        int costGold = 50;
        int costHolyWater = 30;

        if (gold >= costGold && holyWater >= costHolyWater)
        {
            auto building = BuildingManager::getInstance()->createBuilding(selectedType, snapPos);
            if (building)
            {
                this->addChild(building, 5);
                gold -= costGold;
                holyWater -= costHolyWater;
                updateResourceDisplay();
            }
        }
        else
        {
            CCLOG("资源不足，无法建造");
        }
    }

    if (placeModesoldier)
    {
        auto soldier = SoldierManager::getInstance()->createSoldier(selectedType, snapPos);
        if (soldier) this->addChild(soldier, 5);
    }

    placeModebuild = false;
    placeModesoldier = false;
    selectedType = 0;
}
