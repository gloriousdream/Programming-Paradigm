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

    // 1. 背景
    auto bg = Sprite::create("GrassBackground.png");
    bg->setAnchorPoint(Vec2::ZERO);
    bg->setPosition(origin);
    this->addChild(bg, 0);

    // 2. 建筑和兵种按钮
    auto buildBtn = MenuItemImage::create("Building.png", "Building.png", CC_CALLBACK_0(GameScene::onBuildButtonPressed, this));
    auto soldierBtn = MenuItemImage::create("Soldier.png", "Soldier.png", CC_CALLBACK_0(GameScene::onSoldierpushed, this));
    auto menu = Menu::create(buildBtn, soldierBtn, nullptr);
    menu->setPosition(origin.x + visibleSize.width - 120, origin.y + visibleSize.height / 2);
    menu->alignItemsVerticallyWithPadding(50);
    this->addChild(menu, 10);

    // 3. 点击草地放置建筑或士兵
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

    // 4. 监听建筑点击事件
    auto buildingListener = EventListenerCustom::create("BUILDING_CLICKED", [=](EventCustom* event) {
        Sprite* building = (Sprite*)event->getUserData();
        onBuildingClicked(building);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(buildingListener, this);

    return true;
}

// 点击建筑
void GameScene::onBuildingClicked(Sprite* building)
{
    showUpgradeButton(building);
}

// 显示升级按钮
void GameScene::showUpgradeButton(Sprite* building)
{
    this->removeChildByName("UPGRADE_MENU");

    Vec2 pos = building->getPosition() + Vec2(0, 100);

    auto btn = MenuItemImage::create(
        "UpgradeButton.png",
        "UpgradeButton.png",
        [=](Ref*) {
            Building* b = dynamic_cast<Building*>(building);
            if (b) b->upgrade();
            this->removeChildByName("UPGRADE_MENU");
        }
    );

    auto menu = Menu::create(btn, nullptr);
    menu->setPosition(pos);
    menu->setName("UPGRADE_MENU");
    this->addChild(menu, 999);
}

// 建筑按钮
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

// 进入建造/士兵放置模式
template<typename T>
void GameScene::enablePlaceMode(int type, T menu)
{
    if (typeid(*menu) == typeid(BuildMenu)) placeModebuild = true;
    if (typeid(*menu) == typeid(Soldiermenu)) placeModesoldier = true;
    selectedType = type;
}

// 放置建筑/士兵
void GameScene::onMapClicked(Vec2 pos)
{
    if (!(placeModebuild || placeModesoldier)) return;

    int gx = pos.x / 64;
    int gy = pos.y / 64;
    Vec2 snapPos(gx * 64 + 32, gy * 64 + 32);

    if (placeModebuild)
    {
        auto building = BuildingManager::getInstance()->createBuilding(selectedType, snapPos);
        if (building) this->addChild(building, 5);
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
