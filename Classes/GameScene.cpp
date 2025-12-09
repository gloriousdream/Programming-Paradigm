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
// Type: 1=野蛮人, 2=巨人, 3=弓箭手, 4=炸弹人
static SoldierConfig getSoldierConfig(int type)
{
    switch (type)
    {
        case 1: return { "Barbarian", 0, 25, 1 };    // 便宜，占1人口
        case 2: return { "Giant",     0, 200, 5 };   // 贵，肉盾，占5人口
        case 3: return { "Archer",    0, 50, 1 };    // 远程，占1人口
        case 4: return { "Bomber",    0, 100, 2 };   // 战术兵种，占2人口
        default: return { "Unknown",   0, 0, 0 };
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

    // 背景
    auto bg = Sprite::create("GrassBackground.png");
    bg->setAnchorPoint(Vec2::ZERO);
    bg->setPosition(origin);
    this->addChild(bg, 0);

    // 建造、士兵按钮
    auto buildBtn = MenuItemImage::create("Building.png", "Building.png", CC_CALLBACK_0(GameScene::onBuildButtonPressed, this));
    auto soldierBtn = MenuItemImage::create("Soldier.png", "Soldier.png", CC_CALLBACK_0(GameScene::onSoldierpushed, this));
    auto menu = Menu::create(buildBtn, soldierBtn, nullptr);
    menu->setPosition(origin.x + visibleSize.width - 130, origin.y + visibleSize.height / 2);
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

    // 人口显示
    auto popSprite = Sprite::create("Population.png");  // 你需要准备一张图
    popSprite->setPosition(Vec2(origin.x + visibleSize.width - 100,
        origin.y + visibleSize.height - 130));
    this->addChild(popSprite, 10);

    populationLabel = Label::createWithTTF(std::to_string(population),
        "fonts/Marker Felt.ttf", 24);
    populationLabel->setAnchorPoint(Vec2(0, 0.5f));
    populationLabel->setPosition(popSprite->getPosition() + Vec2(20, 0));
    this->addChild(populationLabel, 10);

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
    // 更新金币文字
    if (goldLabel)
    {
        goldLabel->setString(std::to_string(gold));
    }

    // 更新圣水文字
    if (waterLabel)
    {
        waterLabel->setString(std::to_string(holyWater));
    }

    // [新增] 更新人口文字
    if (populationLabel)
    {
        // 显示格式：当前人口 / 上限 (假设100，或者你需要定义 maxPopulation 变量)
        populationLabel->setString(std::to_string(population) + "/100");
    }
}
void GameScene::showMilitaryOptions(cocos2d::Sprite* building)
{
    Vec2 pos = building->getPosition();
    Size buildingSize = building->getContentSize();

    // 转换为 Building* 以便访问 upgradeCost 和 upgrade()
    Building* targetBuilding = static_cast<Building*>(building);

    // --- 1. 创建升级按钮 (左侧) ---
    auto upgradeBtn = MenuItemImage::create(
        "UpgradeButton.png", // 确保有这个图片，或者换成 Upgrade.png
        "UpgradeButton.png",
        [=](Ref* sender)
        {
            // 获取升级所需的资源
            int goldCost = targetBuilding->upgradeCostGold;
            int holyCost = targetBuilding->upgradeCostHoly;

            // 检查资源是否足够
            if (this->gold >= goldCost && this->holyWater >= holyCost)
            {
                // 1. 扣除资源
                this->gold -= goldCost;
                this->holyWater -= holyCost;

                // 2. 执行建筑升级
                targetBuilding->upgrade();

                // 3. 刷新界面资源显示
                this->updateResourceDisplay();

                CCLOG("Upgrade Successful! Level: %d", targetBuilding->getLevel());
            }
            else
            {
                CCLOG("Not enough resources to upgrade!");
                // 这里可选：加一个飘字提示 "资源不足"
            }

            // 无论成功失败，操作后关闭菜单
            this->removeChildByTag(999);
            this->currentBuildingMenu = nullptr;
        }
    );

    // --- 2. 创建造兵按钮 (右侧) ---
    auto trainBtn = MenuItemImage::create(
        "Train.png",
        "Train.png",
        [=](Ref* sender)
        {
            // 打开造兵菜单
            this->showTrainMenu(building);
            // 移除当前操作菜单
            this->removeChildByTag(999);
            this->currentBuildingMenu = nullptr;
        }
    );

    // --- 3. 组装菜单 ---
    auto menu = Menu::create(upgradeBtn, trainBtn, nullptr);

    // 设置位置：分别在建筑头顶的左右两侧
    upgradeBtn->setPosition(Vec2(-50, buildingSize.height / 2 + 50));
    trainBtn->setPosition(Vec2(50, buildingSize.height / 2 + 50));

    menu->setPosition(pos);
    menu->setTag(999);
    this->addChild(menu, 100);

    currentBuildingMenu = building;
}
void GameScene::onBuildingClicked(cocos2d::Sprite* building)
{
    // 1. 如果之前有打开的菜单，先移除（防止重叠）
    this->removeChildByTag(999);

    if (currentBuildingMenu)
    {
        // 如果你有其他清除逻辑放在这里
        currentBuildingMenu = nullptr;
    }

    // 2. 尝试将传入的 building 转换为 MilitaryCamp 指针
    MilitaryCamp* camp = dynamic_cast<MilitaryCamp*>(building);

    if (camp)
    {
        // 转换成功！说明点击的是兵营 -> 显示【升级】和【造兵】双按钮
        showMilitaryOptions(building);
    }
    else
    {
        // 转换失败，说明是普通建筑 -> 只显示【升级】
        showUpgradeButton(building);
    }
}
void GameScene::showTrainMenu(cocos2d::Sprite* building)
{
    // 1. 创建全屏的造兵菜单 Layer
    auto menu = Soldiermenu::createMenu();

    if (!menu)
    {
        CCLOGERROR("Failed to create Soldiermenu");
        return;
    }

    // 2. 设置核心回调逻辑
    // [=] 捕获当前类的指针 this 和 building 指针
    menu->onTrainSoldier = [=](int soldierType, int amount)
        {

            // --- A. 数据校验与资源扣除 ---

            // 获取配置
            SoldierConfig config = getSoldierConfig(soldierType);

            // 计算总价
            int totalGoldCost = config.costGold * amount;
            int totalWaterCost = config.costHolyWater * amount;
            int totalPop = config.popSpace * amount;

            // 检查资源是否足够
            if (this->gold < totalGoldCost || this->holyWater < totalWaterCost)
            {
                CCLOG("Training Failed: Not enough resources!");
                // 这里建议未来加一个浮动提示: "No Resources"
                return;
            }

            // 检查人口限制 (假设上限100)
            if (this->population + totalPop > 100)
            {
                CCLOG("Training Failed: Population limit reached!");
                return;
            }

            // --- B. 执行交易 ---

            this->gold -= totalGoldCost;
            this->holyWater -= totalWaterCost;
            this->population += totalPop;

            // 刷新 UI 上的数字
            this->updateResourceDisplay();


            // --- C. 场景中生成实体 (你刚补充的部分) ---

            // 获取兵营的中心位置
            Vec2 campPos = building->getPosition();

            for (int i = 0; i < amount; i++)
            {
                // 在兵营中心附近随机偏移 (-30 ~ +30 像素)
                // 这样兵就不会全部重叠在一个点上
                float offsetX = (rand() % 60) - 30;
                float offsetY = (rand() % 60) - 30;
                Vec2 spawnPos = campPos + Vec2(offsetX, offsetY);

                // 调用单例管理器创建具体的士兵对象
                auto soldier = SoldierManager::getInstance()->createSoldier(soldierType, spawnPos);

                if (soldier)
                {
                    // 添加到场景
                    // ZOrder 设为 15，确保它能盖住草地(0)和部分建筑(5-10)，如果有层级遮挡需求请动态调整
                    this->addChild(soldier, 15);

                    // 注意：如果你的 Soldier 类实现了我之前给的 actionWalk，
                    // 它们在被 createSoldier 创建出来的那一刻就已经开始自动巡逻了。
                }
            }

            CCLOG("Training Started: Created %d units of Type %d", amount, soldierType);
        };

    // 3. 将菜单层添加到当前场景的最上方 (Z=200 确保盖住所有东西)
    this->addChild(menu, 200);
}// [防报错补丁] 如果你还没有实现这个函数，请加上这个空实现，否则会编译失败
void GameScene::showTrainAmountMenu(int soldierType)
{
    CCLOG("TODO: Show amount selection for soldier type %d", soldierType);
    // 下一步我们将在这里实现滑动条或数量选择界面
}
void GameScene::showUpgradeButton(Sprite* building)
{
    // 删除已有菜单
    if (auto existingMenu = this->getChildByName("UPGRADE_MENU"))
        existingMenu->removeFromParent();

    Building* b = dynamic_cast<Building*>(building);
    if (!b) return;

    Vec2 pos = building->getPosition() + Vec2(0, 100);
    auto upgradeNode = Node::create();
    upgradeNode->setName("UPGRADE_MENU");
    upgradeNode->setPosition(pos);
    this->addChild(upgradeNode, 999);

    if (b->getLevel() >= 3)
    {
        // 已满级提示
        auto fullLabel = Label::createWithTTF("已满级", "fonts/Marker Felt.ttf", 24);
        fullLabel->setColor(Color3B::RED);
        fullLabel->setAnchorPoint(Vec2(0.5f, 0.5f));
        fullLabel->setPosition(Vec2::ZERO);
        upgradeNode->addChild(fullLabel);
    }
    else
    {
        // 升级按钮
        auto btn = MenuItemImage::create(
            "UpgradeButton.png",
            "UpgradeButton.png",
            [=](Ref*) {
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
                upgradeNode->removeFromParent();
                currentBuildingMenu = nullptr;
            }
        );
        auto menu = Menu::create(btn, nullptr);
        menu->setPosition(Vec2::ZERO);
        upgradeNode->addChild(menu);

        // 显示升级资源消耗
        auto goldLabelNode = Label::createWithTTF("G:" + std::to_string(50), "fonts/Marker Felt.ttf", 20);
        goldLabelNode->setAnchorPoint(Vec2(0, 0.5f));
        goldLabelNode->setPosition(Vec2(btn->getContentSize().width / 2 + 10, 0));
        upgradeNode->addChild(goldLabelNode);

        auto waterLabelNode = Label::createWithTTF("H:" + std::to_string(30), "fonts/Marker Felt.ttf", 20);
        waterLabelNode->setAnchorPoint(Vec2(0, 0.5f));
        waterLabelNode->setPosition(Vec2(btn->getContentSize().width / 2 + 10, -25));
        upgradeNode->addChild(waterLabelNode);
    }
}


// 建造按钮
void GameScene::onBuildButtonPressed()
{
    auto existingMenu = this->getChildByName("BUILD_MENU_NODE");
    if (existingMenu)
    {
        existingMenu->removeFromParent();
        return;
    }

    // 创建 BuildMenu
    auto menu = BuildMenu::createMenu();
    menu->setName("BUILD_MENU_NODE"); // 直接给 BuildMenu 命名
    this->addChild(menu, 100);

    menu->onSelectBuilding = [menu, this](int type) {
        enablePlaceMode(type, menu); // 传 BuildMenu 对象，保证判断正确
        menu->removeFromParent();
        };
}


// 士兵按钮
void GameScene::onSoldierpushed()
{
    // 1. 先关闭可能存在的建筑弹窗
    closeCurrentPopup();
    auto menu = Soldiermenu::createMenu();
    this->addChild(menu, 100);

    // 修改这里的回调处理
    menu->onTrainSoldier = [=](int soldierType, int amount)
        {
            CCLOG("Start Training: Type %d, Count %d", soldierType, amount);

            // TODO: 这里写具体的造兵逻辑
            // 1. 检查总金币是否足够 (amount * 单价)
            // 2. 扣钱
            // 3. 添加到造兵队列

            // 示例：简单打印
            // this->startTraining(soldierType, amount); 
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

    // 计算 snapPos 
    int gx = pos.x / 64;
    int gy = pos.y / 64;
    Vec2 snapPos(gx * 64 + 32, gy * 64 + 32);

    if (placeModebuild)
    {
        // 根据 selectedType 确定消耗
        int costGold = 0;
        int costHoly = 0;

        switch (selectedType) {
            case 1: costGold = 50; costHoly = 30; break; // MilitaryCamp
            case 2: costGold = 40; costHoly = 20; break; // WaterCollection
            case 3: costGold = 70; costHoly = 40; break; // ArrowTower
            case 4: costGold = 150; costHoly = 100; break; // TownHall
            case 5: costGold = 20; costHoly = 80; break; // CoinCollection
        }

        if (gold >= costGold && holyWater >= costHoly)
        {
            auto building = BuildingManager::getInstance()->createBuilding(selectedType, snapPos);
            if (building)
            {
                this->addChild(building, 5);
                gold -= costGold;
                holyWater -= costHoly;
                updateResourceDisplay();
                // 建造成功后，是否要关闭放置模式取决于您的设计，如果不希望连续建造，可以加：
                // placeModebuild = false; 
            }
        }
        else
        {
            CCLOG("资源不足");
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
// [新增] 实现关闭弹窗函数
void GameScene::closeCurrentPopup()
{
    if (currentPopup)
    {
        currentPopup->removeFromParent();
        currentPopup = nullptr;
    }
}