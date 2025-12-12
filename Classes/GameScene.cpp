#include "GameScene.h"
#include "BuildMenu.h"
#include "BuildingManager.h"
#include "Soldiermenu.h"
#include "SoldierManager.h"
#include "Building.h"
#include "MilitaryCamp.h"
#include "FightScene.h"

USING_NS_CC;

struct SoldierConfig
{
    std::string name;
    int costGold;      // 金币消耗
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

// 辅助函数：根据建筑类型获取图片路径（用于显示虚影）
static std::string getBuildingTexturePath(int type)
{
    switch (type) {
        case 1: return "MilitaryCamp.png";
        case 2: return "WaterCollection.png";
        case 3: return "ArrowTower.png";
        case 4: return "TownHall.png";
        case 5: return "CoinCollection.png";
        default: return "CloseNormal.png";
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

    // 1. 背景
    auto bg = Sprite::create("GrassBackground.png");
    if (bg) {
        bg->setAnchorPoint(Vec2::ZERO);
        bg->setPosition(origin);
        this->addChild(bg, 0);
    }

    // 2. 建造、战斗按钮
    auto buildBtn = MenuItemImage::create("Building.png", "Building.png", CC_CALLBACK_0(GameScene::onBuildButtonPressed, this));
    auto FightBtn = MenuItemImage::create("Fight.png", "Fight.png", CC_CALLBACK_0(GameScene::onFightpushed, this));
    auto menu = Menu::create(buildBtn, FightBtn, nullptr);
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
        if (placeModebuild || placeModesoldier)
        {
            onMapClicked(t->getLocation());
            return true;
        }
        if (currentPopup) {
            // 可选：点击空地关闭菜单
            // closeCurrentPopup();
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
            // 直接使用 getLocationInView 即可（根据你的反馈不需要翻转）
            Vec2 temp = e->getLocationInView();
            Vec2 mousePos = Vec2(temp.x, temp.y);

            // 网格吸附计算
            int gx = mousePos.x / 64;
            int gy = mousePos.y / 64;
            Vec2 snapPos(gx * 64 + 32, gy * 64 + 32);

            ghostSprite->setPosition(snapPos);
        }
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    // 6. 建筑点击事件
    auto buildingListener = EventListenerCustom::create("BUILDING_CLICKED", [=](EventCustom* event) {
        if (placeModebuild) return; // 建造模式下不响应

        Sprite* building = (Sprite*)event->getUserData();
        onBuildingClicked(building);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(buildingListener, this);

    // 7. 监听收集圣水事件
    auto collectListener = EventListenerCustom::create("COLLECT_WATER_EVENT", [=](EventCustom* event) {
        int* amountPtr = static_cast<int*>(event->getUserData());
        int amount = *amountPtr;
        this->holyWater += amount;
        this->updateResourceDisplay();
        CCLOG("GameScene: HolyWater increased by %d", amount);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(collectListener, this);

    // 8. 监听收集金币事件
    auto collectCoinListener = EventListenerCustom::create("COLLECT_COIN_EVENT", [=](EventCustom* event) {
        int* amountPtr = static_cast<int*>(event->getUserData());
        int amount = *amountPtr;
        this->gold += amount;
        this->updateResourceDisplay();
        CCLOG("GameScene: Gold increased by %d", amount);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(collectCoinListener, this);

    updateResourceDisplay();

    return true;
}

void GameScene::updateResourceDisplay()
{
    if (goldLabel) goldLabel->setString(std::to_string(gold));
    if (waterLabel) waterLabel->setString(std::to_string(holyWater));
    if (populationLabel) populationLabel->setString(std::to_string(population) + "/100");
}

void GameScene::showMilitaryOptions(cocos2d::Sprite* building)
{
    Building* targetBuilding = static_cast<Building*>(building);
    Vec2 pos = building->getPosition();
    Size buildingSize = building->getContentSize();

    // 创建容器
    auto container = Node::create();
    container->setPosition(pos);
    container->setTag(999);
    this->addChild(container, 100);

    // 显示等级
    std::string lvStr = "Lv." + std::to_string(targetBuilding->getLevel());
    auto lvLabel = Label::createWithTTF(lvStr, "fonts/Marker Felt.ttf", 24);
    lvLabel->setColor(Color3B::YELLOW);
    lvLabel->setPosition(Vec2(0, buildingSize.height / 2 + 100));
    container->addChild(lvLabel);

    // 是否满级
    bool isMaxLv = (targetBuilding->getLevel() >= 3);

    // 升级按钮（仅未满级）
    MenuItemImage* upgradeBtn = nullptr;
    if (!isMaxLv)
    {
        upgradeBtn = MenuItemImage::create(
            "UpgradeButton.png", "UpgradeButton.png",
            [=](Ref* sender)
            {
                int goldCost = targetBuilding->upgradeCostGold;
                int holyCost = targetBuilding->upgradeCostHoly;

                if (this->gold >= goldCost && this->holyWater >= holyCost)
                {
                    this->gold -= goldCost;
                    this->holyWater -= holyCost;
                    targetBuilding->upgrade();
                    this->updateResourceDisplay();
                    CCLOG("Upgrade Successful!");
                }
                else
                {
                    CCLOG("Not enough resources!");
                }

                this->removeChildByTag(999);
                this->currentBuildingMenu = nullptr;
            }
        );

        upgradeBtn->setPosition(Vec2(-50, buildingSize.height / 2 + 50));
    }

    // 造兵按钮
    auto trainBtn = MenuItemImage::create(
        "Train.png", "Train.png",
        [=](Ref* sender)
        {
            this->showTrainMenu(building);
            this->removeChildByTag(999);
            this->currentBuildingMenu = nullptr;
        }
    );

    // ⭐ 满级 → 自动居中
    // ⭐ 未满级 → 在右侧显示
    Vec2 trainPos = isMaxLv ?
        Vec2(0, buildingSize.height / 2 + 50) :
        Vec2(50, buildingSize.height / 2 + 50);

    trainBtn->setPosition(trainPos);

    // 创建菜单
    Menu* menu = nullptr;
    if (isMaxLv)
    {
        // 满级 → 只显示造兵按钮
        menu = Menu::create(trainBtn, nullptr);
    }
    else
    {
        // 未满级 → 升级 + 造兵
        menu = Menu::create(upgradeBtn, trainBtn, nullptr);
    }

    menu->setPosition(Vec2::ZERO);
    container->addChild(menu);

    currentBuildingMenu = building;
}

// 统一处理建筑点击事件
void GameScene::onBuildingClicked(cocos2d::Sprite* building)
{
    // 1. 判断逻辑：点击的是否是当前已经打开菜单的那个建筑？
    bool isSameBuilding = (currentBuildingMenu == building);

    // 2. 无论点击的是谁，先清理掉屏幕上现有的菜单
    this->removeChildByTag(999);

    // 双重保险：如果有遗留的 Name 节点也移除
    if (auto node = this->getChildByName("UPGRADE_MENU")) {
        node->removeFromParent();
    }

    // 3. 重置当前选中状态
    currentBuildingMenu = nullptr;

    // 4. 如果是第二次点击同一个建筑，因为上面已经移除了菜单，
    //    这里直接返回，不再执行下面的“显示菜单”逻辑，从而达到“隐藏”的效果。
    if (isSameBuilding) {
        return;
    }

    // 5. 如果是新建筑，则显示对应的菜单
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
    // 1. 清理互斥界面
    this->removeChildByTag(999);
    this->removeChildByTag(998);
    currentBuildingMenu = nullptr;

    auto menu = Soldiermenu::createMenu();
    if (!menu) return;

    menu->setTag(998); // 标记为大弹窗

    menu->onTrainSoldier = [=](int soldierType, int amount)
        {

            // --- 资源检查与扣除逻辑 (保持不变) ---
            SoldierConfig config = getSoldierConfig(soldierType);
            int totalGoldCost = config.costGold * amount;
            int totalWaterCost = config.costHolyWater * amount;
            int totalPop = config.popSpace * amount;

            if (this->gold < totalGoldCost || this->holyWater < totalWaterCost)
            {
                CCLOG("Not enough resources!");
                return;
            }
            if (this->population + totalPop > 100)
            { // 假设上限 100
                CCLOG("Population limit reached!");
                return;
            }

            this->gold -= totalGoldCost;
            this->holyWater -= totalWaterCost;
            this->population += totalPop;
            this->updateResourceDisplay();

            auto visibleSize = Director::getInstance()->getVisibleSize();

            // 1. 计算区域中心点
            // X: 屏幕宽度的 20% 处 (左侧)
            // Y: 屏幕高度的 50% 处 (中间)
            float centerX = visibleSize.width * 0.2f;
            float centerY = visibleSize.height * 0.5f;

            // 2. 定义区域大小 (假设格子是64，2x2就是128)
            float areaW = 128.0f;
            float areaH = 128.0f;

            // 3. 构建矩形 (原点在左下角，所以减去宽高的一半)
            Rect patrolArea(centerX - areaW / 2, centerY - areaH / 2, areaW, areaH);

            // -----------------------------------------------------------
            // [生成士兵]
            // -----------------------------------------------------------
            for (int i = 0; i < amount; i++)
            {
                // 在 patrolArea 这个小方块内随机取一个点作为出生点
                float spawnX = patrolArea.origin.x + CCRANDOM_0_1() * patrolArea.size.width;
                float spawnY = patrolArea.origin.y + CCRANDOM_0_1() * patrolArea.size.height;

                auto soldier = SoldierManager::getInstance()->createSoldier(soldierType, Vec2(spawnX, spawnY));

                if (soldier)
                {
                    // 1. 告诉士兵只能在这个方块里跑
                    soldier->setMoveArea(patrolArea);

                    // 2. 手动启动巡逻
                    soldier->actionWalk();

                    // 3. 添加到场景 (Z轴15)
                    this->addChild(soldier, 15);
                }
            }

            CCLOG("Training Started: %d units spawned at Left-Middle Area", amount);
        };

    this->addChild(menu, 200);
}
void GameScene::showTrainAmountMenu(int soldierType)
{
    CCLOG("TODO: Show amount selection for soldier type %d", soldierType);
}

// 显示普通建筑升级菜单
void GameScene::showUpgradeButton(Sprite* building)
{
    // 移除旧菜单 
    if (auto existingMenu = this->getChildByName("UPGRADE_MENU"))
        existingMenu->removeFromParent();
    this->removeChildByTag(999);

    Building* b = dynamic_cast<Building*>(building);
    if (!b) return;

    Vec2 pos = building->getPosition() + Vec2(0, 100);
    auto upgradeNode = Node::create();
    upgradeNode->setName("UPGRADE_MENU");
    upgradeNode->setTag(999); // 统一 Tag
    upgradeNode->setPosition(pos);
    this->addChild(upgradeNode, 999);

    // 显示等级 Label 
    std::string lvStr = "Lv." + std::to_string(b->getLevel());
    auto lvLabel = Label::createWithTTF(lvStr, "fonts/Marker Felt.ttf", 24);
    lvLabel->setColor(Color3B::YELLOW);
    lvLabel->setPosition(Vec2(0, 50)); // 显示在菜单上方
    upgradeNode->addChild(lvLabel);

    if (b->getLevel() >= 3) {
        auto fullLabel = Label::createWithTTF("已满级", "fonts/Marker Felt.ttf", 24);
        fullLabel->setColor(Color3B::RED);
        upgradeNode->addChild(fullLabel);
    }
    else {
        // 创建升级按钮 
        auto btn = MenuItemImage::create(
            "UpgradeButton.png", "UpgradeButton.png",
            [=](Ref*) {
                // 直接从建筑对象中读取当前等级对应的升级消耗
                int reqG = b->upgradeCostGold;
                int reqW = b->upgradeCostHoly;

                if (gold >= reqG && holyWater >= reqW) {
                    gold -= reqG;
                    holyWater -= reqW;
                    updateResourceDisplay();
                    b->upgrade();
                    CCLOG("升级成功！");
                }
                else {
                    CCLOG("资源不足，无法升级！需要 G:%d W:%d", reqG, reqW);
                }

                // 点击后关闭菜单
                upgradeNode->removeFromParent();
                currentBuildingMenu = nullptr;
            }
        );
        auto menu = Menu::create(btn, nullptr);
        menu->setPosition(Vec2::ZERO);
        upgradeNode->addChild(menu);

        // 显示动态的升级资源消耗文字 
        std::string costGStr = "G:" + std::to_string(b->upgradeCostGold);
        auto goldLabelNode = Label::createWithTTF(costGStr, "fonts/Marker Felt.ttf", 20);
        goldLabelNode->setAnchorPoint(Vec2(0, 0.5f));
        goldLabelNode->setPosition(Vec2(btn->getContentSize().width / 2 + 10, 0));
        upgradeNode->addChild(goldLabelNode);

        // 使用 b->upgradeCostHoly
        std::string costWStr = "H:" + std::to_string(b->upgradeCostHoly);
        auto waterLabelNode = Label::createWithTTF(costWStr, "fonts/Marker Felt.ttf", 20);
        waterLabelNode->setAnchorPoint(Vec2(0, 0.5f));
        waterLabelNode->setPosition(Vec2(btn->getContentSize().width / 2 + 10, -25));
        upgradeNode->addChild(waterLabelNode);
    }
    currentBuildingMenu = building;
}
// 点击建筑建造按钮
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

    menu->onSelectBuilding = [menu, this](int type) {
        this->selectedType = type;
        this->placeModebuild = true;
        this->placeModesoldier = false;

        if (this->ghostSprite) {
            this->ghostSprite->removeFromParent();
            this->ghostSprite = nullptr;
        }

        std::string imgPath = getBuildingTexturePath(type);
        this->ghostSprite = Sprite::create(imgPath);
        if (this->ghostSprite) {
            this->ghostSprite->setOpacity(128);
            auto winSize = Director::getInstance()->getWinSize();
            this->ghostSprite->setPosition(winSize.width / 2, winSize.height / 2);
            this->addChild(this->ghostSprite, 1000);
        }

        CCLOG("已选择建筑 %d, 进入放置模式", type);
        menu->removeFromParent();
        };
}
void GameScene::onFightpushed()
{
    // 如果菜单已经打开了，就把它关掉
    if (this->getChildByTag(TAG_DIFFICULTY_MENU)) {
        this->removeChildByTag(TAG_DIFFICULTY_MENU);
        return;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    float menuX = origin.x + visibleSize.width - 130;
    float menuY = origin.y + visibleSize.height / 2 - 200;

    // --- EASY ---
    auto btnEasy = MenuItemImage::create("Easy.png", "Easy.png", [=](Ref* sender) {
        CCLOG("Go to Easy Mode");

        // 先移除菜单
        this->removeChildByTag(TAG_DIFFICULTY_MENU);

        // 然后再跳转
        Director::getInstance()->pushScene(TransitionFade::create(0.5f, FightScene::createScene(1)));
        });

    // --- MIDDLE ---
    auto btnMiddle = MenuItemImage::create("Middle.png", "Middle.png", [=](Ref* sender) {
        CCLOG("Go to Middle Mode");

        // 先移除菜单
        this->removeChildByTag(TAG_DIFFICULTY_MENU);

        Director::getInstance()->pushScene(TransitionFade::create(0.5f, FightScene::createScene(2)));
        });

    // --- HARD ---
    auto btnHard = MenuItemImage::create("Hard.png", "Hard.png", [=](Ref* sender) {
        CCLOG("Go to Hard Mode");

        // 先移除菜单
        this->removeChildByTag(TAG_DIFFICULTY_MENU);

        Director::getInstance()->pushScene(TransitionFade::create(0.5f, FightScene::createScene(3)));
        });

    // 创建菜单容器
    auto diffMenu = Menu::create(btnEasy, btnMiddle, btnHard, nullptr);

    // 设置菜单整体位置
    diffMenu->setPosition(menuX, menuY);

    // 让按钮垂直对齐，间隔 10 像素 (紧凑一点，看起来像一个下拉列表)
    diffMenu->alignItemsVerticallyWithPadding(10);

    // 设置 Tag，方便后面查找删除
    diffMenu->setTag(TAG_DIFFICULTY_MENU);

    // 添加到场景，层级设为 100 确保在最上层
    this->addChild(diffMenu, 100);
}
void GameScene::onSoldierpushed()
{
    closeCurrentPopup();
    auto menu = Soldiermenu::createMenu();
    this->addChild(menu, 100);

    menu->onTrainSoldier = [=](int soldierType, int amount) {
        CCLOG("Start Training: Type %d, Count %d", soldierType, amount);
        };
}

template<typename T>
void GameScene::enablePlaceMode(int type, T menu)
{
    if (typeid(*menu) == typeid(BuildMenu)) placeModebuild = true;
    if (typeid(*menu) == typeid(Soldiermenu)) placeModesoldier = true;
    selectedType = type;
}

void GameScene::onMapClicked(Vec2 pos)
{
    // 如果不在建造模式也不在放兵模式，直接返回
    if (!(placeModebuild || placeModesoldier)) return;

    // 计算网格吸附位置
    int gx = pos.x / 64;
    int gy = pos.y / 64;
    Vec2 snapPos(gx * 64 + 32, gy * 64 + 32);

    // --- 1. 建造模式逻辑 ---
    if (placeModebuild)
    {
        // 这里需要硬编码一下每种建筑的临时消耗，或者从 Config 获取
        int costGold = 0;
        int costHoly = 0;

        // 简单的判定逻辑 (建议后续封装成 getBuildingCost)
        switch (selectedType)
        {
            case 1: costGold = 50; costHoly = 30; break; // MilitaryCamp
            case 2: costGold = 40; costHoly = 20; break; // WaterCollection
            case 3: costGold = 70; costHoly = 40; break; // ArrowTower
            case 4: costGold = 150; costHoly = 100; break; // TownHall
            case 5: costGold = 20; costHoly = 80; break; // CoinCollection
        }

        // 检查资源
        if (gold >= costGold && holyWater >= costHoly)
        {
            // 尝试创建建筑
            auto building = BuildingManager::getInstance()->createBuilding(selectedType, snapPos);

            if (building)
            {
                // [成功]
                this->addChild(building, 5);
                gold -= costGold;
                holyWater -= costHoly;
                updateResourceDisplay();

                // 移除虚影
                if (ghostSprite)
                {
                    ghostSprite->removeFromParent();
                    ghostSprite = nullptr;
                }
                // 退出模式
                placeModebuild = false;
            }
            else
            {
                // [失败]：位置已被占用
                CCLOG("放置失败：位置已被占用");
                if (ghostSprite)
                {
                    ghostSprite->removeFromParent();
                    ghostSprite = nullptr;
                }
                placeModebuild = false;
            }
        }
        else
        {
            // [失败]：资源不足
            CCLOG("资源不足");
            if (ghostSprite)
            {
                ghostSprite->removeFromParent();
                ghostSprite = nullptr;
            }
            placeModebuild = false;
        }
    }

    // --- 2. 士兵放置逻辑 (进攻模式) ---
    else if (placeModesoldier)
    {
        // 进攻用的兵，不需要限制区域，直接生成并启动
        auto soldier = SoldierManager::getInstance()->createSoldier(selectedType, pos);
        if (soldier)
        {
            soldier->actionWalk();
            this->addChild(soldier, 15);
        }
    }
}
void GameScene::closeCurrentPopup()
{
    if (currentPopup) {
        currentPopup->removeFromParent();
        currentPopup = nullptr;
    }
}