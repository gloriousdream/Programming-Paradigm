#include "GameScene.h"
#include "BuildMenu.h"
#include "BuildingManager.h"
#include "Soldiermenu.h"
#include "SoldierManager.h"
USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

void GameScene::onBuildingClicked(Sprite* building)
{
    showUpgradeButton(building);
}

void GameScene::showUpgradeButton(Sprite* building)
{
    this->removeChildByName("UPGRADE_MENU");

    Vec2 pos = building->getPosition() + Vec2(0, 100);

    auto btn = MenuItemImage::create(
        "UpgradeButton.png",
        "UpgradeButton.png",
        [=](Ref*) {

            int level = building->getTag();   // 当前等级
            level++;                           // 升级
            building->setTag(level);

            // 读取建筑类型
            std::string name = building->getName(); // BUILDING_1
            int type = atoi(name.substr(9).c_str());  // 提取数字部分

            // ============================
            //  不同建筑 → 不同升级逻辑
            // ============================
            if (type == 1)  // 军营
            {
                if (level == 1) building->setTexture("MilitaryCamp.png");
                else if (level == 2) building->setTexture("MilitaryCamp.png");
                else if (level == 3) building->setTexture("MilitaryCamp.png");
            }
            else if (type == 2)  // 水井
            {
                if (level == 1) building->setTexture("WaterCollection.png");
                else if (level == 2) building->setTexture("WaterCollection.png");
                else if (level == 3) building->setTexture("WaterCollection.png");
            }
            else if (type == 3) // 箭塔
            {
                if (level == 1) building->setTexture("ArrowTower.png");
                else if (level == 2) building->setTexture("ArrowTower2.png");
                else if (level == 3) building->setTexture("ArrowTower2.png");
            }

            // 删除升级按钮
            this->removeChildByName("UPGRADE_MENU");
        }
    );

    auto menu = Menu::create(btn, nullptr);
    menu->setPosition(pos);
    menu->setName("UPGRADE_MENU");
    this->addChild(menu, 999);
}



bool GameScene::init()
{
    // =======================================
    // 监听建筑点击事件
    // =======================================
    auto listener = EventListenerCustom::create("BUILDING_CLICKED", [=](EventCustom* event) {
        Sprite* building = (Sprite*)event->getUserData();
        onBuildingClicked(building);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);


    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // -----------------------------
    // 1. 添加草地背景
    // -----------------------------
    auto bg = Sprite::create("GrassBackground.png");

    if (bg == nullptr)
    {
        printf("Error loading 'GrassBackground.png'\n");
    }
    else
    {
        bg->setAnchorPoint(Vec2::ZERO);
        bg->setPosition(Vec2(origin.x, origin.y));
        this->addChild(bg, 0);
    }

    // -----------------------------
    // 2. 添加右侧“建筑”按钮
    // -----------------------------
    auto buildBtn = MenuItemImage::create(
        "Building.png",
        "Building.png",
        CC_CALLBACK_0(GameScene::onBuildButtonPressed, this)
    );
    auto soldier = MenuItemImage::create(
        "Soldier.png",
        "Soldier.png",
        CC_CALLBACK_0(GameScene::onSoldierpushed, this)
    );
    auto menu = Menu::create(buildBtn, soldier, nullptr);
    menu->setPosition(origin.x + visibleSize.width - 120, origin.y + visibleSize.height / 2);
    menu->alignItemsVerticallyWithPadding(50);
    this->addChild(menu, 10);
    // 3. 监听点击草地（建造模式使用）
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [&](Touch* t, Event* e)
        {
            if (placeModebuild)
            {
                onMapClicked(t->getLocation());
                return true;
            }
            if (placeModesoldier)
            {
                onMapClicked(t->getLocation());
            }
            return false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // =======================================
    // 监听建筑点击事件
    // =======================================
    auto buildingListener = EventListenerCustom::create("BUILDING_CLICKED", [=](EventCustom* event) {
        Sprite* building = (Sprite*)event->getUserData();
        onBuildingClicked(building);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(buildingListener, this);


    return true;
}
// ======================================================
//点击兵种按钮，可以查看我现有的兵种
// ======================================================

void GameScene::onSoldierpushed()
{
    auto menu = Soldiermenu::createMenu();
    this->addChild(menu, 100);
    //用户选择战士
    menu->onSelectSoldier = [menu, this](int type)
        {
            enablePlaceMode(type, menu);
            menu->removeFromParent();
        };
}
// ======================================================
//         点击“建筑”按钮 → 弹出建筑选择菜单
// ======================================================
void GameScene::onBuildButtonPressed()
{
    auto menu = BuildMenu::createMenu();
    this->addChild(menu, 100);

    // 用户选了某个建筑
    menu->onSelectBuilding = [menu, this](int type)
        {
            enablePlaceMode(type, menu);
            menu->removeFromParent();
        };
}

// ======================================================
//                 进入建造//士兵创建模式
// ======================================================
template<typename T>
void GameScene::enablePlaceMode(int type, T menu)//传入menu参数，使用typeid来进行类型的比较
{
    if (typeid(*menu) == typeid(BuildMenu))
    {
        placeModebuild = true;
    }
    if (typeid(*menu) == typeid(Soldiermenu))
    {
        placeModesoldier = true;
    }
    selectedType = type;

    CCLOG("进入建造模式，建筑类型 = %d", type);
}

// ======================================================
//        点击草地 → 自动吸附到 64×64 格子并放置建筑/或者兵种
// ======================================================
void GameScene::onMapClicked(Vec2 pos)
{
    if (!(placeModebuild || placeModesoldier)) return;

    // 格子坐标（64×64）
    int gx = (int)(pos.x / 64);
    int gy = (int)(pos.y / 64);

    Vec2 snapPos = Vec2(gx * 64 + 32, gy * 64 + 32);

    // 创建建筑
    if (placeModebuild)
    {
        auto building = BuildingManager::getInstance()->createBuilding(selectedType, snapPos);
        if (building)
        {
            this->addChild(building, 5);
        }
    }
    if (placeModesoldier)
    {
        auto soldier = SoldierManager::getInstance()->createSoldier(selectedType, snapPos);
        if (soldier)
        {
            this->addChild(soldier, 5);
        }
    }
    //创建兵种

    // 退出建造模式
    placeModebuild = false;
    placeModesoldier = false;
    selectedType = 0;
}