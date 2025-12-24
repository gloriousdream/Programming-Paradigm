#include "GameScene.h"
#include "BuildMenu.h"
#include "BuildingManager.h"
#include "Soldiermenu.h"
#include "SoldierManager.h"
#include "Building.h"
#include "MilitaryCamp.h"
#include "FightScene.h"
#include "TownHall.h"
#include "GoldStage.h"    // 金库
#include "ElixirTank.h"   // 圣水罐
#include "Cannon.h"       // 加农炮 
#include "ArrowTower.h"   // 箭塔 
#include "CoinCollection.h"    
#include "WaterCollection.h"  
#include "Boom.h"
// 4.0 
#include "AudioEngine.h"
USING_NS_CC;
int GameScene::gold = 1000;       // 初始金币
int GameScene::holyWater = 500;   // 初始圣水
int GameScene::gems = 10;
// 辅助函数：在左侧区域随机位置生成一个闲置士兵
void GameScene::spawnHomeSoldier(int type)
{
    // 1. 获取原始区域
    Rect area = BuildingManager::getInstance()->getSoldierSpawnArea();

    float tileSize = 64.0f;
    area.origin.x -= tileSize; // X 减小 (向左)
    area.origin.y -= tileSize; // Y 减小 (向下)

    // 2. 在这个区域内随机生成一个坐标
    // 稍微向内收缩一点(padding)，防止贴在边缘
    float padding = 10.0f;
    float minX = area.origin.x + padding;
    float maxX = area.origin.x + area.size.width - padding;
    float minY = area.origin.y + padding;
    float maxY = area.origin.y + area.size.height - padding;

    // 防止区域太小导致崩溃 (安全检查)
    if (maxX <= minX) maxX = minX + 1;
    if (maxY <= minY) maxY = minY + 1;

    // 随机计算 X 和 Y
    float randX = minX + (rand() % static_cast<int>(maxX - minX));
    float randY = minY + (rand() % static_cast<int>(maxY - minY));
    Vec2 spawnPos(randX, randY);

    // 3. 创建士兵
    auto soldier = SoldierManager::getInstance()->createSoldier(type, spawnPos);

    if (soldier)
    {
        soldier->setMoveArea(area);

        // 设置为“家里的兵”，让它开始闲逛
        soldier->actionWalk();

        // 4. 添加到场景
        this->addChild(soldier, 15);
    }
}void GameScene::onExit()
{
    // 退出场景前，自动保存
    this->saveData();
    Scene::onExit();
}

void GameScene::showSkipButton(cocos2d::Sprite* building)
{
    // 1. 清理旧菜单
    if (auto existingMenu = this->getChildByName("UPGRADE_MENU"))
        existingMenu->removeFromParent();
    this->removeChildByTag(999);

    Building* b = dynamic_cast<Building*>(building);
    if (!b) return;

    // 2. 创建容器 Node
    Vec2 pos = building->getPosition() + Vec2(0, 100);
    auto upgradeNode = Node::create();
    upgradeNode->setName("UPGRADE_MENU");
    upgradeNode->setTag(999);
    upgradeNode->setPosition(pos);
    this->addChild(upgradeNode, 999);

    // 3. 显示顶部状态文字
    auto statusLabel = Label::createWithTTF("Upgrading...", "fonts/Marker Felt.ttf", 24);
    statusLabel->setColor(Color3B::GREEN);
    statusLabel->setPosition(Vec2(0, 50));
    upgradeNode->addChild(statusLabel);

    // 4. 创建加速按钮
    auto btn = MenuItemImage::create(
        "accelerate.png", "accelerate.png",
        [=](Ref*)
        {
            // 检查宝石是否足够
            if (gems >= 1)
            {
                // 1. 扣除宝石
                gems--;

                // 2. 刷新右上角的 UI 显示
                if (gemLabel)
                {
                    gemLabel->setString(std::to_string(gems));
                }

                // 3. 执行原有跳过逻辑
                CCLOG("Skip Button Clicked! Spent 1 Gem.");
                b->skipUpgradeTimer();

                // 4. 保存数据
                this->saveData();

                // 5. 关闭菜单
                upgradeNode->removeFromParent();
                currentBuildingMenu = nullptr;
            }
            else
            {
                // 宝石不足的处理
                CCLOG("Not enough gems!");

                // 可选：给个简单的提示动画
                statusLabel->setString("Need Gems!");
                statusLabel->setColor(Color3B::RED);
            }
        }
    );

    auto menu = Menu::create(btn, nullptr);
    menu->setPosition(Vec2::ZERO);
    upgradeNode->addChild(menu);

    // 更新当前选中
    currentBuildingMenu = building;
}
void GameScene::saveData()
{
    auto userDefault = UserDefault::getInstance();

    // 1. 保存资源
    userDefault->setIntegerForKey("PlayerGold", gold);
    userDefault->setIntegerForKey("PlayerHolyWater", holyWater);

    // 保存宝石数量
    userDefault->setIntegerForKey("PlayerGems", gems);

    // 2. 保存士兵数量
    for (int i = 1; i <= 4; i++)
    {
        std::string key = "Soldier_" + std::to_string(i);
        userDefault->setIntegerForKey(key.c_str(), _homeSoldiers[i]);
    }

    // 3. 保存建筑
    std::string buildData = "";
    for (auto child : _children)
    {
        Building* b = dynamic_cast<Building*>(child);
        if (b)
        {
            std::string typeStr = "";

            // 判断建筑类型，必须涵盖所有建筑！
            if (dynamic_cast<TownHall*>(b)) typeStr = "TownHall";
            else if (dynamic_cast<MilitaryCamp*>(b)) typeStr = "MilitaryCamp";
            else if (dynamic_cast<ArrowTower*>(b)) typeStr = "ArrowTower";
            else if (dynamic_cast<Cannon*>(b)) typeStr = "Cannon";
            else if (dynamic_cast<GoldStage*>(b)) typeStr = "GoldStage";
            else if (dynamic_cast<ElixirTank*>(b)) typeStr = "ElixirTank";
            else if (dynamic_cast<CoinCollection*>(b)) typeStr = "CoinCollection";
            else if (dynamic_cast<WaterCollection*>(b)) typeStr = "WaterCollection";
            else if (dynamic_cast<Boom*>(b)) typeStr = "Boom";
            // 如果找到了对应的类型字符串，就拼接到存档里
            if (!typeStr.empty())
            {
                int lv = b->getLevel();
                int x = (int)b->getPositionX();
                int y = (int)b->getPositionY();

                std::string segment = typeStr + "," + std::to_string(lv) + "," +
                    std::to_string(x) + "," + std::to_string(y) + ";";
                buildData += segment;
            }
        }
    }

    userDefault->setStringForKey("BuildingData", buildData);
    userDefault->setBoolForKey("HasSaveData", true);

    userDefault->flush(); // 强制写入文件
    CCLOG("Game Saved! Gems: %d", gems);
}
Sprite* GameScene::createBuildingByName(std::string name, int level)
{
    Building* building = nullptr;

    // 根据名字判断类型
    if (name == "TownHall") building = TownHall::create();
    else if (name == "GoldStage") building = GoldStage::create();
    else if (name == "ElixirTank") building = ElixirTank::create();
    else if (name == "Cannon") building = Cannon::create();
    else if (name == "MilitaryCamp") building = MilitaryCamp::create();
    else if (name == "ArrowTower") building = ArrowTower::create();
    else if (name == "CoinCollection") building = CoinCollection::create();
    else if (name == "WaterCollection") building = WaterCollection::create();
    else if (name == "Boom")  building= Boom::create();
    if (building)
    {
        // 简单循环升级
        for (int i = 1; i < level; i++)
        {
            building->upgrade();
        }
    }
    return building;
}
void GameScene::loadData()
{
    auto userDefault = UserDefault::getInstance();

    // 1. 检查存档
    bool hasSave = userDefault->getBoolForKey("HasSaveData", false);
    if (!hasSave)
    {
        CCLOG("No save data found.");
        return;
    }

    // 2. 恢复资源
    gold = userDefault->getIntegerForKey("PlayerGold", 1000);
    holyWater = userDefault->getIntegerForKey("PlayerHolyWater", 500);
    gems = userDefault->getIntegerForKey("PlayerGems", 10);

    // 3. 恢复士兵
    int totalPopulation = 0;
    for (int i = 1; i <= 4; i++)
    {
        std::string key = "Soldier_" + std::to_string(i);
        int count = userDefault->getIntegerForKey(key.c_str(), 0);

        if (count > 0)
        {
            // 恢复数据
            addGlobalSoldierCount(i, count);
            // 恢复画面
            for (int k = 0; k < count; k++)
            {
                this->spawnHomeSoldier(i);
            }
            int singleSoldierPop = 1; // 默认占1个
            if (i == 1)      singleSoldierPop = 1;  
            else if (i == 2) singleSoldierPop = 5;  
            else if (i == 3) singleSoldierPop = 1;  
            else if (i == 4) singleSoldierPop = 2; 

            // 计算公式： 总人口 += 兵的数量 * 单个兵的人口
            totalPopulation += (count * singleSoldierPop);
        }
    }

    this->population = totalPopulation;
    this->updateResourceDisplay();

    if (gemLabel)
    {
        gemLabel->setString(std::to_string(gems));
    }

    // 4. 恢复建筑
    std::string buildData = userDefault->getStringForKey("BuildingData", "");
    if (!buildData.empty())
    {
        std::stringstream ss(buildData);
        std::string segment;
        while (std::getline(ss, segment, ';'))
        {
            if (segment.empty()) continue;
            std::stringstream ss2(segment);
            std::string typeStr, levelStr, xStr, yStr;

            if (!std::getline(ss2, typeStr, ',')) continue;
            if (!std::getline(ss2, levelStr, ',')) continue;
            if (!std::getline(ss2, xStr, ',')) continue;
            if (!std::getline(ss2, yStr, ',')) continue;

            int level = std::atoi(levelStr.c_str());
            int x = std::atoi(xStr.c_str());
            int y = std::atoi(yStr.c_str());
            if (level < 1) level = 1;

            // 创建建筑
            Sprite* b = createBuildingByName(typeStr, level);
            if (b)
            {
                b->setPosition(Vec2(x, y));
                this->addChild(b, 5);

                // 手动把名字转换成 ID，以便占位
                // 对应 BuildingManager 里的 createBuilding ID
                int typeID = 0;
                if (typeStr == "MilitaryCamp") typeID = 1;
                else if (typeStr == "WaterCollection") typeID = 2;
                else if (typeStr == "ArrowTower") typeID = 3;
                else if (typeStr == "TownHall") typeID = 4;
                else if (typeStr == "CoinCollection") typeID = 5;
                else if (typeStr == "Cannon") typeID = 6;
                else if (typeStr == "GoldStage") typeID = 7;
                else if (typeStr == "ElixirTank") typeID = 8;
                else if (typeStr == "Boom") typeID = 9;
                // 只有当 ID 有效时才去占位
                if (typeID > 0)
                {
                    BuildingManager::getInstance()->occupyGrid(Vec2(x, y), typeID);
                }
            }
        }
    }

    CCLOG("Game Loaded! Gems: %d", gems);
}
std::map<int, int> GameScene::_globalSoldiers;
std::map<int, int> GameScene::_homeSoldiers;

int GameScene::getGlobalSoldierCount(int type)
{
    // 如果没有这个key，自动返回0
    if (_globalSoldiers.find(type) == _globalSoldiers.end()) return 0;
    return _globalSoldiers[type];
}

void GameScene::addGlobalSoldierCount(int type, int amount)
{
    if (amount > 0)
    {
        _homeSoldiers[type] += amount;
        // 同时同步给显示用的库存
        _globalSoldiers[type] = _homeSoldiers[type];
    }
    else
    {
        // 只扣除当前战斗用的临时库存
        _globalSoldiers[type] += amount;
        // 防止减成负数
        if (_globalSoldiers[type] < 0) _globalSoldiers[type] = 0;
    }
}

struct SoldierConfig
{
    std::string name;
    int costGold;      // 金币消耗
    int costHolyWater; // 圣水消耗
    int popSpace;      // 占用人口
};
int GameScene::getGlobalGold()
{
    return gold;
}

int GameScene::getGlobalHolyWater()
{
    return holyWater;
}

void GameScene::addGlobalResources(int addGold, int addHoly)
{
    gold += addGold;
    holyWater += addHoly;

    // 简单的防溢出或防负数保护
    if (gold < 0) gold = 0;
    if (holyWater < 0) holyWater = 0;
}
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
        case 6: return "cannon_stand.png";
        case 7: return "gold_stage_05.png";
        case 8: return "elixir_tank_05.png";
        case 9: return "boom1.png";
        default: return "CloseNormal.png";
    }
}
void GameScene::onEnter()
{
    // 先调用父类的 onEnter 
    Scene::onEnter();
    cocos2d::AudioEngine::stopAll();

    // 播放家园背景音乐
    cocos2d::AudioEngine::play2d("scene_music.mp3", true, 0.5f);
    // 每次回到这个场景，都强制刷新一次 UI
    this->updateResourceDisplay();
    this->saveData();
    CCLOG("Safe Save: Entered Home Scene."); // 打印日志方便验证
}
Scene* GameScene::createScene()
{
    return GameScene::create();
}

void GameScene::update(float dt)
{
    _dayNightTimer += dt;
    // 利用 sin 函数计算透明度 (0~150之间波动)
    float opacity = 75 + 75 * sin(_dayNightTimer * 0.5f);
    if (_nightLayer) {
        _nightLayer->setOpacity(static_cast<GLubyte>(opacity));
    }
}
bool GameScene::init()
{
    if (!Scene::init()) return false;
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 1. 创建下雨效果
    auto rain = ParticleRain::create();

    // 2. 设置位置（屏幕顶部中间）
    rain->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2,
        Director::getInstance()->getVisibleSize().height));

    // 3. 设置发射范围（覆盖全屏宽度）
    rain->setPosVar(Vec2(Director::getInstance()->getVisibleSize().width / 2, 0));

    // 调整雨滴大小和密度
    // 1. 调大雨滴尺寸 (默认大约是 4.0 左右，改成 10-15 会很明显)
    rain->setStartSize(12.0f);      // 初始大小
    rain->setStartSizeVar(4.0f);    // 大小随机波动范围 (有的8，有的16，看起来自然)
    rain->setEndSize(12.0f);        // 落地时的大小
    rain->setEndSizeVar(4.0f);

    // 2. 增加雨滴数量/密度
    rain->setTotalParticles(1000);  

    // 3. 加快下落速度 
    rain->setSpeed(800);            // 速度调快
    rain->setSpeedVar(100);

    rain->setLife(3); // 存活时间，保证雨滴能落到底部
    this->addChild(rain, 999);

    // 昼夜交替
    // 1. 开启 update 调用
    this->scheduleUpdate();
    // 2. 创建黑色遮罩 (初始全透明)
    // 这里的 100 是层级 (Z-Order)，要保证比建筑高(>5)，但比 UI 低(UI通常很高)
    _nightLayer = LayerColor::create(Color4B(0, 0, 20, 0));
    this->addChild(_nightLayer, 100);

    // 背景
    auto bg = Sprite::create("GrassBackground.png");
    if (bg) {
        bg->setAnchorPoint(Vec2::ZERO);
        bg->setPosition(origin);
        this->addChild(bg, 0);
    }

    // 我的温馨小屋
    auto labelHouse = Label::createWithSystemFont("My Base Camp", "Arial", 40);
    labelHouse->setColor(Color3B::YELLOW); // 设为黄色显眼一点
    labelHouse->enableOutline(Color4B::BLACK, 2); // 加个描边
    labelHouse->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 50));
    this->addChild(labelHouse, 1000); // 层级设高点，防止被挡住

    auto myHouseSprite = Sprite::create("MyHouse.png");
    if (myHouseSprite) {
        // 计算位置：放在文字右侧 60 像素处
        float offsetX = labelHouse->getContentSize().width / 2 + 60;
        myHouseSprite->setPosition(labelHouse->getPosition() + Vec2(offsetX, 0));

        this->addChild(myHouseSprite, 1000);
    }
    else {
        // 如果没图，输出个日志防止崩溃
        CCLOG("Error: MyHouse.png not found!");
    }

    // 建造、战斗按钮
    auto buildBtn = MenuItemImage::create("Building.png", "Building.png", CC_CALLBACK_0(GameScene::onBuildButtonPressed, this));
    auto FightBtn = MenuItemImage::create("Fight.png", "Fight.png", CC_CALLBACK_0(GameScene::onFightpushed, this));
    auto menu = Menu::create(buildBtn, FightBtn, nullptr);
    menu->setPosition(origin.x + visibleSize.width - 80, origin.y + visibleSize.height / 2);
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
    auto popSprite = Sprite::create("Population.png");
    popSprite->setPosition(Vec2(origin.x + visibleSize.width - 100, origin.y + visibleSize.height - 130));
    this->addChild(popSprite, 10);

    populationLabel = Label::createWithTTF(std::to_string(population), "fonts/Marker Felt.ttf", 24);
    populationLabel->setAnchorPoint(Vec2(0, 0.5f));
    populationLabel->setPosition(popSprite->getPosition() + Vec2(20, 0));
    this->addChild(populationLabel, 10);

    auto gemSprite = Sprite::create("gem.png"); // 
    gemSprite->setPosition(Vec2(origin.x + visibleSize.width - 100, origin.y + visibleSize.height - 170)); // 放在人口下面 (-130 再减 40)
    this->addChild(gemSprite, 10);

    gemLabel = Label::createWithTTF(std::to_string(gems), "fonts/Marker Felt.ttf", 24);
    gemLabel->setAnchorPoint(Vec2(0, 0.5f));
    gemLabel->setPosition(gemSprite->getPosition() + Vec2(20, 0));
    this->addChild(gemLabel, 10);
    // 地图点击事件 
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [=](Touch* t, Event* e) {
        if (placeModebuild || placeModesoldier)
        {
            onMapClicked(t->getLocation());
            return true;
        }
        if (currentPopup) {
            // closeCurrentPopup();
        }
        return false;
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // 鼠标移动监听
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = [=](Event* event) {
        if (placeModebuild && ghostSprite)
        {
            EventMouse* e = static_cast<EventMouse*>(event);
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

    // 建筑点击事件
    auto buildingListener = EventListenerCustom::create("BUILDING_CLICKED", [=](EventCustom* event) {
        if (placeModebuild) return; // 建造模式下不响应

        Sprite* building = static_cast<Sprite*>(event->getUserData());
        onBuildingClicked(building);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(buildingListener, this);

    // 监听收集圣水事件
    auto collectListener = EventListenerCustom::create("COLLECT_WATER_EVENT", [=](EventCustom* event) {
        int* amountPtr = static_cast<int*>(event->getUserData());
        int amount = *amountPtr;
        this->holyWater += amount;
        this->updateResourceDisplay();
        CCLOG("GameScene: HolyWater increased by %d", amount);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(collectListener, this);

    // 监听收集金币事件
    auto collectCoinListener = EventListenerCustom::create("COLLECT_COIN_EVENT", [=](EventCustom* event) {
        int* amountPtr = static_cast<int*>(event->getUserData());
        int amount = *amountPtr;
        this->gold += amount;
        this->updateResourceDisplay();
        CCLOG("GameScene: Gold increased by %d", amount);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(collectCoinListener, this);
    updateResourceDisplay();
    this->loadData();
    return true;
}

void GameScene::updateResourceDisplay()
{
    if (goldLabel) goldLabel->setString(std::to_string(gold));
    if (waterLabel) waterLabel->setString(std::to_string(holyWater));
    if (populationLabel) populationLabel->setString(std::to_string(population) + "/100");
    int playerMaxGold = 1000;
    int playerMaxHoly = 1000;
    // 遍历场景所有子节点
    auto children = this->getChildren();
    for (auto node : children)
    {
        // 尝试将其转换为 GoldStage
        auto goldStage = dynamic_cast<GoldStage*>(node);
        if (goldStage)
        {
            // 如果转换成功，说明这个建筑是金库，调用更新函数
            goldStage->updateVisuals(this->gold, playerMaxGold);
        }
        auto elixirTank = dynamic_cast<ElixirTank*>(node);
        if (elixirTank)
        {
            // 传入当前的 holyWater 和 上限
            elixirTank->updateVisuals(this->holyWater, playerMaxHoly);
        }
    }
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

    // 升级按钮
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
                    this->updateResourceDisplay();
                    targetBuilding->startUpgradeTimer(90.0f);
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

    // 满级自动居中
    // 未满级在右侧显示
    Vec2 trainPos = isMaxLv ?
        Vec2(0, buildingSize.height / 2 + 50) :
        Vec2(50, buildingSize.height / 2 + 50);

    trainBtn->setPosition(trainPos);

    // 创建菜单
    Menu* menu = nullptr;
    if (isMaxLv)
    {
        // 满级只显示造兵按钮
        menu = Menu::create(trainBtn, nullptr);
    }
    else
    {
        // 未满级显示升级 + 造兵
        menu = Menu::create(upgradeBtn, trainBtn, nullptr);
    }

    menu->setPosition(Vec2::ZERO);
    container->addChild(menu);

    currentBuildingMenu = building;
}

// 统一处理建筑点击事件
void GameScene::onBuildingClicked(cocos2d::Sprite* building)
{
    // 判断逻辑：点击的是否是当前已经打开菜单的那个建筑？
    bool isSameBuilding = (currentBuildingMenu == building);

    // 无论点击的是谁，先清理掉屏幕上现有的菜单
    this->removeChildByTag(999);

    // 如果有遗留的 Name 节点也移除
    if (auto node = this->getChildByName("UPGRADE_MENU")) {
        node->removeFromParent();
    }

    // 重置当前选中状态
    currentBuildingMenu = nullptr;

    // 如果是第二次点击同一个建筑，因为上面已经移除了菜单，
    // 直接返回，不再执行下面的“显示菜单”逻辑，从而达到“隐藏”的效果。
    if (isSameBuilding) {
        return;
    }
    Building* target = dynamic_cast<Building*>(building);
    if (!target) return;
    int tag = target->getTag();
    bool isMaxLv = (target->getLevel() >= 3);
    // 如果是新建筑，则显示对应的菜单
    MilitaryCamp* camp = dynamic_cast<MilitaryCamp*>(building);
    if (tag)
    {
        // 兵营特殊处理
        if (camp)
        {
            showMilitaryOptions(building);
        }
        else
        {
            showUpgradeButton(building); // 走原来的老路
        }
    }
    else if (!tag)
    {
        CCLOG("正在升级中，显示跳过按钮");
        showSkipButton(building); // 走新的路
    }
}

void GameScene::showTrainMenu(cocos2d::Sprite* building)
{
    // 清理互斥界面
    this->removeChildByTag(999);
    this->removeChildByTag(998);
    currentBuildingMenu = nullptr;

    auto menu = Soldiermenu::createMenu();
    if (!menu) return;

    menu->setTag(998); // 标记为大弹窗

    menu->onTrainSoldier = [=](int soldierType, int amount)
        {

            // 资源检查与扣除逻辑 
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

            // 更新全局兵力库存
            addGlobalSoldierCount(soldierType, amount);

            CCLOG("已存入兵库: 类型%d, 数量%d, 当前库存%d",
                soldierType, amount, getGlobalSoldierCount(soldierType));

            auto visibleSize = Director::getInstance()->getVisibleSize();

            // 计算区域中心点
            // X: 屏幕宽度的 20% 处 (左侧)
            // Y: 屏幕高度的 50% 处 (中间)
            float centerX = visibleSize.width * 0.2f;
            float centerY = visibleSize.height * 0.5f;

            // 定义区域大小
            float areaW = 128.0f;
            float areaH = 128.0f;

            // 构建矩形 (原点在左下角，所以减去宽高的一半)
            Rect patrolArea(centerX - areaW / 2, centerY - areaH / 2, areaW, areaH);

            // 生成士兵
            for (int i = 0; i < amount; i++)
            {
                // 在 patrolArea 这个小方块内随机取一个点作为出生点
                float spawnX = patrolArea.origin.x + CCRANDOM_0_1() * patrolArea.size.width;
                float spawnY = patrolArea.origin.y + CCRANDOM_0_1() * patrolArea.size.height;

                auto soldier = SoldierManager::getInstance()->createSoldier(soldierType, Vec2(spawnX, spawnY));

                if (soldier)
                {
                    // 告诉士兵只能在这个方块里跑
                    soldier->setMoveArea(patrolArea);

                    // 手动启动巡逻
                    soldier->actionWalk();

                    // 添加到场景 
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
                    b->startUpgradeTimer(90.0f);
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
    // 防止重复打开菜单
    auto existingMenu = this->getChildByName("BUILD_MENU_NODE");
    if (existingMenu) {
        existingMenu->removeFromParent();
        return;
    }

    // 创建建造菜单
    auto menu = BuildMenu::createMenu();
    menu->setName("BUILD_MENU_NODE");
    this->addChild(menu, 100);

    // 定义菜单点击回调
    menu->onSelectBuilding = [menu, this](int type) {
        this->selectedType = type;

        // 进入建造模式
        this->placeModebuild = true;
        this->placeModesoldier = false;

        // 移除旧虚影
        if (this->ghostSprite) {
            this->ghostSprite->removeFromParent();
            this->ghostSprite = nullptr;
        }

        if (type == 6) {
            CCLOG("GameScene: 正在创建加农炮虚影...");

            // 必须先加载 plist
            SpriteFrameCache::getInstance()->addSpriteFramesWithFile("atlas.plist");

            // 优先使用底座 
            this->ghostSprite = Sprite::createWithSpriteFrameName("cannon_stand.png");

            // 如果找不到底座，尝试找炮管
            if (!this->ghostSprite) {
                CCLOG("GameScene: 没找到底座，尝试使用炮管...");
                this->ghostSprite = Sprite::createWithSpriteFrameName("cannon01.png");
            }

            // 如果找不到，创建一个显眼的红色方块
            if (!this->ghostSprite) {
                CCLOG("GameScene: 【严重错误】无法创建虚影！显示红色方块警告。");
                this->ghostSprite = Sprite::create();
                this->ghostSprite->setTextureRect(Rect(0, 0, 80, 80)); // 80x80的大方块
                this->ghostSprite->setColor(Color3B::RED);
            }
        }
        else {
            // 其他普通建筑 
            std::string imgPath = getBuildingTexturePath(type);
            this->ghostSprite = Sprite::create(imgPath);

            // 普通建筑的保底
            if (!this->ghostSprite) {
                this->ghostSprite = Sprite::create();
                this->ghostSprite->setTextureRect(Rect(0, 0, 60, 60));
                this->ghostSprite->setColor(Color3B::MAGENTA); // 紫色方块表示普通图片缺失
            }
        }

        // 设置虚影位置
        if (this->ghostSprite) {
            this->ghostSprite->setOpacity(128); // 半透明

            // 使用 getVisibleSize 确保一定在屏幕视野正中间
            auto visibleSize = Director::getInstance()->getVisibleSize();
            Vec2 origin = Director::getInstance()->getVisibleOrigin();

            // 屏幕中心坐标 = 原点 + 宽高的一半
            Vec2 centerPos = Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2);

            this->ghostSprite->setPosition(centerPos);
            this->addChild(this->ghostSprite, 1000);

            CCLOG("虚影已创建，位置: (%f, %f)", centerPos.x, centerPos.y);
        }

        // 关闭菜单
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

    float menuX = origin.x + visibleSize.width - 80;
    float menuY = origin.y + visibleSize.height / 2 - 230;

    // EASY 
    auto btnEasy = MenuItemImage::create("Easy.png", "Easy.png", [=](Ref* sender) {
        CCLOG("Go to Easy Mode");

        GameScene::_globalSoldiers = GameScene::_homeSoldiers;

        // 先移除菜单
        this->removeChildByTag(TAG_DIFFICULTY_MENU);

        // 然后再跳转
        Director::getInstance()->pushScene(TransitionFade::create(0.5f, FightScene::createScene(1)));
        });

    // MIDDLE
    auto btnMiddle = MenuItemImage::create("Middle.png", "Middle.png", [=](Ref* sender) {
        CCLOG("Go to Middle Mode");

        GameScene::_globalSoldiers = GameScene::_homeSoldiers;

        // 先移除菜单
        this->removeChildByTag(TAG_DIFFICULTY_MENU);

        Director::getInstance()->pushScene(TransitionFade::create(0.5f, FightScene::createScene(2)));
        });

    // HARD 
    auto btnHard = MenuItemImage::create("Hard.png", "Hard.png", [=](Ref* sender) {
        CCLOG("Go to Hard Mode");

        GameScene::_globalSoldiers = GameScene::_homeSoldiers;

        // 先移除菜单
        this->removeChildByTag(TAG_DIFFICULTY_MENU);

        Director::getInstance()->pushScene(TransitionFade::create(0.5f, FightScene::createScene(3)));
        });

    // 创建菜单容器
    auto diffMenu = Menu::create(btnEasy, btnMiddle, btnHard, nullptr);

    // 设置菜单整体位置
    diffMenu->setPosition(menuX, menuY);

    // 让按钮垂直对齐，间隔 10 像素
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

    // 建造模式
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
            case 6: costGold = 200; costHoly = 0; break; // Cannon
        }

        // 检查资源
        if (gold >= costGold && holyWater >= costHoly)
        {
            // 尝试创建建筑
            auto building = BuildingManager::getInstance()->createBuilding(selectedType, snapPos);

            if (building)
            {
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
            // 资源不足
            CCLOG("资源不足");
            if (ghostSprite)
            {
                ghostSprite->removeFromParent();
                ghostSprite = nullptr;
            }
            placeModebuild = false;
        }
    }

    // 士兵放置
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