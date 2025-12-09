#include "GameScene.h"
#include "BuildMenu.h"
#include "BuildingManager.h"
#include "Soldiermenu.h"
#include "SoldierManager.h"
#include "Building.h"
#include "MilitaryCamp.h"

USING_NS_CC;

// --- 在 GameScene.cpp 顶部添加配置 ---

struct SoldierConfig
{
    std::string name;
    int costGold;      // 金币消耗 (通常主要消耗圣水)
    int costHolyWater; // 圣水消耗
    int popSpace;      // 占用人口
};

// 辅助函数：获取兵种配置
static SoldierConfig getSoldierConfig(int type)
{
    switch (type)
    {
        case 1: return { "Barbarian", 0, 25, 1 };
        case 2: return { "Giant",     0, 200, 5 };
        case 3: return { "Archer",    0, 50, 1 };
        case 4: return { "Bomber",    0, 100, 2 };
        default: return { "Unknown",   0, 0, 0 };
    }
}

// 【新增】辅助函数：根据建筑类型获取图片路径（用于显示虚影）
// 必须和你 BuildMenu 里的图片对应
static std::string getBuildingTexturePath(int type)
{
    switch (type) {
        case 1: return "MilitaryCamp.png";
        case 2: return "WaterCollection.png";
        case 3: return "ArrowTower.png";
        case 4: return "TownHall.png";
        case 5: return "CoinCollection.png";
    }
}

Scene* GameScene::createScene()
{
    return GameScene::create();
}

bool GameScene::init()
{
    if (!Scene::init()) return false;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 1. 背景 (保留你的 GrassBackground.png)
    auto bg = Sprite::create("GrassBackground.png");
    if (bg) { // 加个判断防止空指针
        bg->setAnchorPoint(Vec2::ZERO);
        bg->setPosition(origin);
        this->addChild(bg, 0);
    }

    // 2. 建造、士兵按钮
    auto buildBtn = MenuItemImage::create("Building.png", "Building.png", CC_CALLBACK_0(GameScene::onBuildButtonPressed, this));
    auto soldierBtn = MenuItemImage::create("Soldier.png", "Soldier.png", CC_CALLBACK_0(GameScene::onSoldierpushed, this));
    auto menu = Menu::create(buildBtn, soldierBtn, nullptr);
    menu->setPosition(origin.x + visibleSize.width - 130, origin.y + visibleSize.height / 2);
    menu->alignItemsVerticallyWithPadding(50);
    this->addChild(menu, 10);

    // 3. 资源显示
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

    // 人口显示
    auto popSprite = Sprite::create("Population.png");
    popSprite->setPosition(Vec2(origin.x + visibleSize.width - 100, origin.y + visibleSize.height - 130));
    this->addChild(popSprite, 10);

    populationLabel = Label::createWithTTF(std::to_string(population), "fonts/Marker Felt.ttf", 24);
    populationLabel->setAnchorPoint(Vec2(0, 0.5f));
    populationLabel->setPosition(popSprite->getPosition() + Vec2(20, 0));
    this->addChild(populationLabel, 10);

    // 4. 地图点击事件 (用于确认放置)
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [=](Touch* t, Event* e) {
        // 如果处于建造模式或造兵模式，且点击了地图
        if (placeModebuild || placeModesoldier)
        {
            onMapClicked(t->getLocation());
            return true; // 吞噬事件
        }

        // 点击空白处关闭弹窗
        if (currentPopup) {
            // closeCurrentPopup(); // 可选：如果你想点空地关闭菜单
        }
        return false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // 5. 鼠标移动监听
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = [=](Event* event) {
        if (placeModebuild && ghostSprite)
        {
            EventMouse* e = (EventMouse*)event;
            auto visibleSize = Director::getInstance()->getVisibleSize();

            // =========== 调试区域：如果方向反了，请切换注释 ===========

             Vec2 temp = e->getLocationInView();
             Vec2 mousePos = Vec2(temp.x, temp.y); // 不做任何Y轴翻转，直接用

            // 网格吸附计算 (保持不变)
            int gx = mousePos.x / 64;
            int gy = mousePos.y / 64;
            Vec2 snapPos(gx * 64 + 32, gy * 64 + 32);

            ghostSprite->setPosition(snapPos);
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    // 6. 建筑点击事件
    auto buildingListener = EventListenerCustom::create("BUILDING_CLICKED", [=](EventCustom* event) {
        // 如果正在放置建筑，禁止点击其他建筑
        if (placeModebuild) return;

        Sprite* building = (Sprite*)event->getUserData();
        onBuildingClicked(building);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(buildingListener, this);

    return true;
}

// 刷新右上角资源显示
void GameScene::updateResourceDisplay()
{
    if (goldLabel) goldLabel->setString(std::to_string(gold));
    if (waterLabel) waterLabel->setString(std::to_string(holyWater));
    if (populationLabel) populationLabel->setString(std::to_string(population) + "/100");
}

void GameScene::showMilitaryOptions(cocos2d::Sprite* building)
{
    Vec2 pos = building->getPosition();
    Size buildingSize = building->getContentSize();
    Building* targetBuilding = static_cast<Building*>(building);

    // --- 升级按钮 ---
    auto upgradeBtn = MenuItemImage::create(
        "UpgradeButton.png", "UpgradeButton.png",
        [=](Ref* sender) {
            int goldCost = targetBuilding->upgradeCostGold;
            int holyCost = targetBuilding->upgradeCostHoly;

            if (this->gold >= goldCost && this->holyWater >= holyCost) {
                this->gold -= goldCost;
                this->holyWater -= holyCost;
                targetBuilding->upgrade();
                this->updateResourceDisplay();
                CCLOG("Upgrade Successful!");
            }
            else {
                CCLOG("Not enough resources!");
            }
            this->removeChildByTag(999);
            this->currentBuildingMenu = nullptr;
        }
    );

    // --- 造兵按钮 ---
    auto trainBtn = MenuItemImage::create(
        "Train.png", "Train.png",
        [=](Ref* sender) {
            this->showTrainMenu(building);
            this->removeChildByTag(999);
            this->currentBuildingMenu = nullptr;
        }
    );

    auto menu = Menu::create(upgradeBtn, trainBtn, nullptr);
    upgradeBtn->setPosition(Vec2(-50, buildingSize.height / 2 + 50));
    trainBtn->setPosition(Vec2(50, buildingSize.height / 2 + 50));
    menu->setPosition(pos);
    menu->setTag(999);
    this->addChild(menu, 100);
    currentBuildingMenu = building;
}

void GameScene::onBuildingClicked(cocos2d::Sprite* building)
{
    this->removeChildByTag(999);
    if (currentBuildingMenu) currentBuildingMenu = nullptr;

    MilitaryCamp* camp = dynamic_cast<MilitaryCamp*>(building);
    if (camp) {
        showMilitaryOptions(building);
    }
    else {
        showUpgradeButton(building);
    }
}

void GameScene::showTrainMenu(cocos2d::Sprite* building)
{
    auto menu = Soldiermenu::createMenu();
    if (!menu) return;

    menu->onTrainSoldier = [=](int soldierType, int amount) {
        SoldierConfig config = getSoldierConfig(soldierType);
        int totalGoldCost = config.costGold * amount;
        int totalWaterCost = config.costHolyWater * amount;
        int totalPop = config.popSpace * amount;

        if (this->gold < totalGoldCost || this->holyWater < totalWaterCost) {
            CCLOG("Not enough resources!");
            return;
        }
        if (this->population + totalPop > 100) {
            CCLOG("Population limit reached!");
            return;
        }

        this->gold -= totalGoldCost;
        this->holyWater -= totalWaterCost;
        this->population += totalPop;
        this->updateResourceDisplay();

        Vec2 campPos = building->getPosition();
        for (int i = 0; i < amount; i++) {
            float offsetX = (rand() % 60) - 30;
            float offsetY = (rand() % 60) - 30;
            Vec2 spawnPos = campPos + Vec2(offsetX, offsetY);
            auto soldier = SoldierManager::getInstance()->createSoldier(soldierType, spawnPos);
            if (soldier) this->addChild(soldier, 15);
        }
        };
    this->addChild(menu, 200);
}

void GameScene::showTrainAmountMenu(int soldierType)
{
    CCLOG("TODO: Show amount selection for soldier type %d", soldierType);
}

void GameScene::showUpgradeButton(Sprite* building)
{
    if (auto existingMenu = this->getChildByName("UPGRADE_MENU"))
        existingMenu->removeFromParent();

    Building* b = dynamic_cast<Building*>(building);
    if (!b) return;

    Vec2 pos = building->getPosition() + Vec2(0, 100);
    auto upgradeNode = Node::create();
    upgradeNode->setName("UPGRADE_MENU");
    upgradeNode->setPosition(pos);
    this->addChild(upgradeNode, 999);

    if (b->getLevel() >= 3) {
        auto fullLabel = Label::createWithTTF("已满级", "fonts/Marker Felt.ttf", 24);
        fullLabel->setColor(Color3B::RED);
        upgradeNode->addChild(fullLabel);
    }
    else {
        auto btn = MenuItemImage::create(
            "UpgradeButton.png", "UpgradeButton.png",
            [=](Ref*) {
                int reqG = 50; int reqW = 30; // 简化的数值
                if (gold >= reqG && holyWater >= reqW) {
                    gold -= reqG; holyWater -= reqW;
                    updateResourceDisplay();
                    b->upgrade();
                }
                upgradeNode->removeFromParent();
                currentBuildingMenu = nullptr;
            }
        );
        auto menu = Menu::create(btn, nullptr);
        menu->setPosition(Vec2::ZERO);
        upgradeNode->addChild(menu);

        // 显示价格Label逻辑省略以节省篇幅，如需保留请复制原逻辑
    }
}

// --- 建造按钮 (点击打开菜单) ---
void GameScene::onBuildButtonPressed()
{
    auto existingMenu = this->getChildByName("BUILD_MENU_NODE");
    if (existingMenu) {
        existingMenu->removeFromParent();
        return;
    }

    auto menu = BuildMenu::createMenu();
    menu->setName("BUILD_MENU_NODE");
    this->addChild(menu, 100);

    // 【核心修改】点击菜单中的图标后
    menu->onSelectBuilding = [menu, this](int type) {

        // 1. 设置状态
        this->selectedType = type;
        this->placeModebuild = true;
        this->placeModesoldier = false;

        // 2. 清除旧虚影（如果有）
        if (this->ghostSprite) {
            this->ghostSprite->removeFromParent();
            this->ghostSprite = nullptr;
        }

        // 3. 创建新虚影 (Semi-transparent Ghost)
        std::string imgPath = getBuildingTexturePath(type);
        this->ghostSprite = Sprite::create(imgPath);
        if (this->ghostSprite) {
            this->ghostSprite->setOpacity(128); // 50%透明度
            // 初始位置设在屏幕中心，之后会随鼠标移动
            auto winSize = Director::getInstance()->getWinSize();
            this->ghostSprite->setPosition(winSize.width / 2, winSize.height / 2);
            this->addChild(this->ghostSprite, 1000); // 放在最上层
        }

        CCLOG("已选择建筑 %d, 进入放置模式", type);

        // 4. 关闭菜单
        menu->removeFromParent();
        };
}

// 士兵按钮
void GameScene::onSoldierpushed()
{
    closeCurrentPopup();
    auto menu = Soldiermenu::createMenu();
    this->addChild(menu, 100);

    menu->onTrainSoldier = [=](int soldierType, int amount) {
        // 这里只是打印，具体逻辑见 showTrainMenu
        CCLOG("Start Training: Type %d, Count %d", soldierType, amount);
        };
}

// 启用放置模式 (此函数现在被 onBuildButtonPressed 的 lambda 替代，保留以防兼容)
template<typename T>
void GameScene::enablePlaceMode(int type, T menu)
{
    if (typeid(*menu) == typeid(BuildMenu)) placeModebuild = true;
    if (typeid(*menu) == typeid(Soldiermenu)) placeModesoldier = true;
    selectedType = type;
}

// --- 点击地图放置 (确认放置) ---
void GameScene::onMapClicked(Vec2 pos)
{
    if (!(placeModebuild || placeModesoldier)) return;

    // 计算网格位置
    int gx = pos.x / 64;
    int gy = pos.y / 64;
    Vec2 snapPos(gx * 64 + 32, gy * 64 + 32);

    // 1. 建筑放置逻辑
    if (placeModebuild)
    {
        // 检查资源
        int costGold = 0;
        int costHoly = 0;
        switch (selectedType) {
            case 1: costGold = 50; costHoly = 30; break;
            case 2: costGold = 40; costHoly = 20; break;
            case 3: costGold = 70; costHoly = 40; break;
            case 4: costGold = 150; costHoly = 100; break;
            case 5: costGold = 20; costHoly = 80; break;
        }

        if (gold >= costGold && holyWater >= costHoly)
        {
            // 尝试创建实体
            auto building = BuildingManager::getInstance()->createBuilding(selectedType, snapPos);
            if (building)
            {
                this->addChild(building, 5);
                gold -= costGold;
                holyWater -= costHoly;
                updateResourceDisplay();

                // 【核心修改】放置成功后，移除虚影
                if (ghostSprite) {
                    ghostSprite->removeFromParent();
                    ghostSprite = nullptr;
                }

                // 关闭放置模式
                placeModebuild = false;
            }
            else {
                CCLOG("放置失败：位置已被占用");
            }
        }
        else {
            CCLOG("资源不足");
        }
    }

    // 2. 士兵放置逻辑 (如需)
    if (placeModesoldier)
    {
        auto soldier = SoldierManager::getInstance()->createSoldier(selectedType, snapPos);
        if (soldier) this->addChild(soldier, 5);
        placeModesoldier = false;
    }

    selectedType = 0;
}

void GameScene::closeCurrentPopup()
{
    if (currentPopup) {
        currentPopup->removeFromParent();
        currentPopup = nullptr;
    }
}