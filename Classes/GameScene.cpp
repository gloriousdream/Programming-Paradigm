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

bool GameScene::init()
{
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
    auto menu = Menu::create(buildBtn,soldier, nullptr);
    menu->setPosition(origin.x + visibleSize.width - 120, origin.y + visibleSize.height / 2);
    menu->alignItemsVerticallyWithPadding(50);
    this->addChild(menu, 10);
    // -----------------------------
    // 3. 监听点击草地（建造模式使用）
    // -----------------------------
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [&](Touch* t, Event* e)
        {
            if (placeModebuild)
            {
                onMapClicked(t->getLocation());
                return true;
            }
            if(placeModesoldier)
            {
                onMapClicked(t->getLocation());
            }
            return false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

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
            enablePlaceMode(type,menu);
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
            enablePlaceMode(type,menu);
            menu->removeFromParent();
        };
}

// ======================================================
//                 进入建造//士兵创建模式
// ======================================================
template<typename T>
void GameScene::enablePlaceMode(int type, T menu)//传入menu参数，使用typeid来进行类型的比较
{
    if(typeid(*menu)==typeid(BuildMenu))
    {
        placeModebuild = true;
    }
    if(typeid(*menu) == typeid(Soldiermenu))
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
    if (!(placeModebuild ||placeModesoldier)) return;

    // 格子坐标（64×64）
    int gx = (int)(pos.x / 64);
    int gy = (int)(pos.y / 64);

    Vec2 snapPos = Vec2(gx * 64 + 32, gy * 64 + 32);

    // 创建建筑
    if(placeModebuild)
    {
        auto building = BuildingManager::getInstance()->createBuilding(selectedType, snapPos);
        if (building)
        {
            this->addChild(building, 5);
        }
    }
    if(placeModesoldier)
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
