#include "Scene/FightScene.h"
#include "Scene/GameScene.h"
#include <string>
#include <ctime>  
#include <cstdlib> 
#include "Soldier/SoldierManager.h"
#include "Building/BuildingManager.h"
#include "AudioEngine.h"
USING_NS_CC;

Scene* FightScene::createScene(int difficulty)
{
    return FightScene::create(difficulty);
}

FightScene* FightScene::create(int difficulty)
{
    FightScene* pRet = new(std::nothrow) FightScene();
    if (pRet && pRet->initWithDifficulty(difficulty))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

// 回放场景创建入口
FightScene* FightScene::createReplayScene(const ReplayData& data)
{
    FightScene* pRet = new(std::nothrow) FightScene();
    if (pRet && pRet->initForReplay(data))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

// 辅助：判断格子是否有效
bool FightScene::isValidGrid(int x, int y)
{
    return x >= 0 && x < 30 && y >= 0 && y < 16;
}

// 辅助：判断格子是否是墙
bool FightScene::isGridBlocked(int x, int y)
{
    if (!isValidGrid(x, y)) return true;
    return mapGrid[x][y]; // true 代表被占用
}

// 寻找最佳攻击站位
Vec2 FightScene::findBestAttackPosition(Vec2 startPos, Building* targetBuilding)
{
    if (!targetBuilding) return startPos;

    // 获取建筑的格子信息
    Vec2 targetCenter = targetBuilding->getPosition();
    int gridX = targetCenter.x / TILE_SIZE;
    int gridY = targetCenter.y / TILE_SIZE;

    int size = 2;
    if (dynamic_cast<TownHall*>(targetBuilding)) size = 3;

    // 搜索建筑外围一圈的格子
    Vec2 bestPos = startPos; // 默认失败返回原点
    float minDst = 999999.0f;

    // 遍历外围 (从 gridX-1 到 gridX+size)
    for (int x = gridX - 1; x <= gridX + size; x++)
    {
        for (int y = gridY - 1; y <= gridY + size; y++)
        {
            // 跳过建筑内部
            if (x >= gridX && x < gridX + size && y >= gridY && y < gridY + size) continue;

            // 如果这个格子是有效的且为空
            if (isValidGrid(x, y) && !mapGrid[x][y])
            {
                Vec2 worldPos = Vec2(x * TILE_SIZE + TILE_SIZE / 2, y * TILE_SIZE + TILE_SIZE / 2);
                float dst = startPos.distance(worldPos);
                if (dst < minDst)
                {
                    minDst = dst;
                    bestPos = worldPos;
                }
            }
        }
    }
    return bestPos;
}

// A* 寻路算法
std::vector<Vec2> FightScene::findPath(Vec2 startWorldPos, Vec2 targetWorldPos)
{
    std::vector<Vec2> path;

    // 安全拦截：如果起点已经在地图外，根本不用算路，直接返回
    if (!isValidGrid(startWorldPos.x / TILE_SIZE, startWorldPos.y / TILE_SIZE)) {
        CCLOG("Start pos is out of bounds!");
        return path;
    }

    // 1. 坐标转换 (世界 -> 格子)
    int startX = startWorldPos.x / TILE_SIZE;
    int startY = startWorldPos.y / TILE_SIZE;
    int targetX = targetWorldPos.x / TILE_SIZE;
    int targetY = targetWorldPos.y / TILE_SIZE;

    // 如果起点终点重合，直接返回
    if (startX == targetX && startY == targetY) return path;

    // 2. 初始化 OpenList 和 ClosedList
    std::vector<AStarNode*> openList;
    std::vector<AStarNode*> closedList;

    AStarNode* startNode = new AStarNode(startX, startY);
    openList.push_back(startNode);

    AStarNode* foundTarget = nullptr;

    // 方向数组：上 下 左 右
    int dir[4][2] = { {0,1}, {0,-1}, {-1,0}, {1,0} };

    while (!openList.empty())
    {
        // 找 F 值最小的节点
        auto it = openList.begin();
        AStarNode* current = *it;
        for (auto i = openList.begin(); i != openList.end(); ++i)
        {
            if ((*i)->getF() < current->getF())
            {
                current = *i;
                it = i;
            }
        }

        openList.erase(it);
        closedList.push_back(current);

        // 到达目标？
        if (current->x == targetX && current->y == targetY)
        {
            foundTarget = current;
            break;
        }

        // 遍历邻居
        for (int i = 0; i < 4; i++)
        {
            int nx = current->x + dir[i][0];
            int ny = current->y + dir[i][1];

            if (isGridBlocked(nx, ny)) continue;

            // 是否在 CloseList
            bool inClosed = false;
            for (auto node : closedList)
            {
                if (node->x == nx && node->y == ny) inClosed = true;
            }
            if (inClosed) continue;

            // 是否在 OpenList
            AStarNode* neighbor = nullptr;
            for (auto node : openList)
            {
                if (node->x == nx && node->y == ny) neighbor = node;
            }

            int newG = current->g + 1; // 假设每格代价 1

            if (neighbor == nullptr)
            {
                neighbor = new AStarNode(nx, ny);
                neighbor->g = newG;
                neighbor->h = abs(nx - targetX) + abs(ny - targetY); // 曼哈顿距离
                neighbor->parent = current;
                openList.push_back(neighbor);
            }
            else if (newG < neighbor->g)
            {
                neighbor->g = newG;
                neighbor->parent = current;
            }
        }
    }

    // 3. 构建路径 (回溯)
    if (foundTarget)
    {
        AStarNode* curr = foundTarget;
        while (curr)
        {
            // 转回世界坐标中心点
            path.push_back(Vec2(curr->x * TILE_SIZE + TILE_SIZE / 2, curr->y * TILE_SIZE + TILE_SIZE / 2));
            curr = curr->parent;
        }
        std::reverse(path.begin(), path.end()); // 反转，变成从起点到终点
    }

    // 清理内存 
    for (auto n : openList) delete n;
    for (auto n : closedList) delete n;
    // foundTarget 在 closedList 或 openList 里，会被清理

    return path;
}

void FightScene::showDeployMenu()
{
    if (_deployMenuLayer) return;

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 1. 半透明遮罩
    _deployMenuLayer = LayerColor::create(Color4B(0, 0, 0, 180));
    this->addChild(_deployMenuLayer, 200);

    // 吞噬点击，防止透传
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch*, Event*) { return true; };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, _deployMenuLayer);

    // 2. 标题
    auto title = Label::createWithSystemFont("Select Unit to Deploy", "Arial", 32);
    title->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 150));
    _deployMenuLayer->addChild(title);

    // 定义兵种数据
    struct SoldierOption
    {
        std::string img;
        int type;
        std::string name;
    };

    std::vector<SoldierOption> options = {
        {"yemanren_select.png", 1, "Barbarian"},
        {"juren_select.png",2, "Giant"},
        {"gongjianshou_select.png",3, "Archer"},
        {"boom_select.png",4, "Bomber"}
    };

    Vector<MenuItem*> items;
    for (const auto& opt : options)
    {
        auto item = MenuItemImage::create(
            opt.img,
            opt.img,
            [=](Ref* sender)
            {
                this->onSelectSoldier(opt.type);
            }
        );

        item->setScale(1.2f);

        // 在图标下方加名字
        int count = GameScene::getGlobalSoldierCount(opt.type);
        std::string nameStr = opt.name + " x" + std::to_string(count);

        // 颜色提示：如果没有兵了，名字显示灰色，否则白色
        auto nameLabel = Label::createWithSystemFont(nameStr, "Arial", 20);
        nameLabel->setPosition(Vec2(item->getContentSize().width / 2, -20));

        if (count <= 0) {
            nameLabel->setColor(Color3B::GRAY);
            item->setOpacity(100); // 图标变暗
        }
        else {
            nameLabel->setColor(Color3B::WHITE);
        }

        item->addChild(nameLabel);
        items.pushBack(item);
    }

    // 3. 关闭按钮 (X)
    auto closeLabel = Label::createWithSystemFont("Cancel", "Arial", 28);
    closeLabel->setColor(Color3B::RED);
    auto closeItem = MenuItemLabel::create(closeLabel, [=](Ref*)
        {
            this->hideDeployMenu();
        });
    closeItem->setPosition(Vec2(visibleSize.width / 2, 100));

    // 4. 将兵种按钮放入菜单
    auto menu = Menu::createWithArray(items);
    menu->alignItemsHorizontallyWithPadding(50);
    menu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

    auto closeMenu = Menu::create(closeItem, nullptr);
    closeMenu->setPosition(Vec2::ZERO);

    _deployMenuLayer->addChild(menu);
    _deployMenuLayer->addChild(closeMenu);
}

void FightScene::initBattleUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    auto item = MenuItemImage::create(
        "AttackButton.png",
        "AttackButton.png",
        [=](Ref* sender)
        {
            this->showDeployMenu();
        }
    );

    // 设置按钮在屏幕左中的位置
    item->setPosition(Vec2(visibleSize.width - 100, visibleSize.height / 2));

    // 创建菜单容器并添加进去
    auto menu = Menu::create(item, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 100);
}

void FightScene::onSelectSoldier(int type)
{
    // 1. 记录选中的兵种
    _selectedSoldierType = type;

    CCLOG("Selected Soldier Type: %d", type);

    // 2. 关闭界面
    hideDeployMenu();
}

// 添加士兵的接口
void FightScene::addSoldier(Soldier* soldier)
{
    if (!soldier) return;

    // 1. 加到场景显示 (Z轴设为15，盖在地面上)
    this->addChild(soldier, 15);

    // 2. 加到逻辑列表 (给加农炮索敌用)
    _mySoldiers.pushBack(soldier);
}

// 执行放兵逻辑
void FightScene::executeDeploySoldier(int soldierType, Vec2 pos)
{
    // 非回放模式下才扣库存
    if (!_isReplayMode) {
        GameScene::addGlobalSoldierCount(soldierType, -1);
    }

    // 创建
    auto soldier = SoldierManager::getInstance()->createSoldier(soldierType, pos);
    if (soldier)
    {
        // 加农炮识别到这个士兵
        this->addSoldier(soldier);

        // 1. 找攻击目标
        Building* target = getPriorityTarget(pos);

        if (target)
        {
            soldier->setTargetBuilding(target);

            // 2. 找落脚点 (目标建筑旁边的空地)
            Vec2 attackPos = findBestAttackPosition(pos, target);

            // 3.算路径 (A*)
            std::vector<Vec2> path = findPath(pos, attackPos);

            // 4. 把路径给士兵
            if (!path.empty())
            {
                soldier->setPath(path);
            }
            else
            {
                CCLOG("No path found! Soldier might be stuck.");
            }
        }
    }
}

// 校验和录制
void FightScene::onMapClick(Vec2 pos)
{
    // 回放模式下禁止点击
    if (_isReplayMode) return;

    if (_selectedSoldierType == 0) return;

    // 检查库存是否充足
    int currentStock = GameScene::getGlobalSoldierCount(_selectedSoldierType);
    if (currentStock <= 0)
    {
        CCLOG("兵力不足！无法放置兵种 %d", _selectedSoldierType);
        return; // 库存不足，直接返回，不执行下面的放兵逻辑
    }

    // 1. 执行放兵
    executeDeploySoldier(_selectedSoldierType, pos);
    CCLOG("放置成功！剩余库存: %d", currentStock - 1);

    // 2. 录制操作
    ReplayActionData action;
    action.time = 120.0f - _timeLeft; // 记录发生的时间点 (从0开始递增)
    action.soldierType = _selectedSoldierType;
    action.x = pos.x;
    action.y = pos.y;
    _currentRecord.actions.push_back(action);
}

void FightScene::hideDeployMenu()
{
    if (_deployMenuLayer)
    {
        _deployMenuLayer->removeFromParent();
        _deployMenuLayer = nullptr;
    }
}

bool FightScene::init()
{
    if (!Scene::init()) return false;
    return true;
}
void FightScene::updateResourceUI()
{
    // 1. 获取全局互通数据
    int totalGold = GameScene::getGlobalGold();
    int totalHoly = GameScene::getGlobalHolyWater();

    // 2. 更新 Label 显示
    if (_goldLabel)
    {
        _goldLabel->setString(std::to_string(totalGold));
    }
    if (_holyLabel)
    {
        _holyLabel->setString(std::to_string(totalHoly));
    }
}
void FightScene::initTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [](Touch* t, Event* e) { return true; };
    listener->onTouchEnded = [=](Touch* t, Event* e)
        {
            // 转换坐标
            Vec2 pos = t->getLocation();
            this->onMapClick(pos);
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

bool FightScene::initWithDifficulty(int difficulty)
{
    // 1. 必须先调用父类初始化
    if (!Scene::init()) return false;
    cocos2d::AudioEngine::stopAll(); // 停止家园音乐
    cocos2d::AudioEngine::play2d("fight_music.mp3", true, 0.5f); // 循环播放战斗音乐
    SoldierManager::getInstance()->reset();
    BuildingManager::getInstance()->reset();

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

    // 清理 FightScene 自己的变量
    _mySoldiers.clear();
    _enemyBuildings.clear();
    memset(mapGrid, 0, sizeof(mapGrid)); // 清空地图墙壁记录

    // 初始化录像数据
    _isReplayMode = false;
    _currentRecord.actions.clear();
    _currentRecord.difficulty = difficulty;

    // 生成随机种子
    unsigned int seed = (unsigned int)time(nullptr);
    std::srand(seed);           // 播种
    _currentRecord.seed = seed; // 保存种子

    _difficulty = difficulty;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 2. 基础环境

    // 背景
    auto bg = Sprite::create("GrassBackground.png");
    if (bg)
    {
        bg->setAnchorPoint(Vec2::ZERO);
        bg->setPosition(origin);
        float scaleX = visibleSize.width / bg->getContentSize().width;
        float scaleY = visibleSize.height / bg->getContentSize().height;
        bg->setScale(std::max(scaleX, scaleY));
        this->addChild(bg, 0);
    }

    // 难度文字
    std::string diffText = "";
    if (_difficulty == 1) diffText = "Mode: EASY";
    else if (_difficulty == 2) diffText = "Mode: MIDDLE";
    else if (_difficulty == 3) diffText = "Mode: HARD";

    auto label = Label::createWithSystemFont(diffText, "Arial", 32);
    label->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 50));
    label->setColor(Color3B::RED);
    this->addChild(label, 100);

    // 返回按钮
    MenuItem* closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        [](Ref* sender) {
            Director::getInstance()->popScene();
        });

    if (closeItem == nullptr || closeItem->getContentSize().width == 0)
    {
        auto lbl = Label::createWithSystemFont("Back", "Arial", 30);
        closeItem = MenuItemLabel::create(lbl, [](Ref*) {
            Director::getInstance()->popScene();
            });
    }
    closeItem->setPosition(Vec2(origin.x + visibleSize.width - 50, origin.y + 50));
    auto menu = Menu::create(closeItem, nullptr);
    menu->setPosition(Vec2::ZERO);

    menu->setTag(9999);

    this->addChild(menu, 100);

    // 3. 资源 UI (金币/圣水)
    auto goldSprite = Sprite::create("GoldCoin.png");
    if (goldSprite) {
        goldSprite->setPosition(Vec2(origin.x + visibleSize.width - 100, origin.y + visibleSize.height - 50));
        this->addChild(goldSprite, 20);
    }

    int currentGold = GameScene::getGlobalGold();
    _goldLabel = Label::createWithTTF(std::to_string(currentGold), "fonts/Marker Felt.ttf", 24);
    if (_goldLabel) {
        _goldLabel->setAnchorPoint(Vec2(0, 0.5f));
        if (goldSprite) _goldLabel->setPosition(goldSprite->getPosition() + Vec2(20, 0));
        else _goldLabel->setPosition(Vec2(visibleSize.width - 80, visibleSize.height - 50));
        this->addChild(_goldLabel, 20);
    }

    auto waterSprite = Sprite::create("HolyWater.png");
    if (waterSprite) {
        waterSprite->setPosition(Vec2(origin.x + visibleSize.width - 100, origin.y + visibleSize.height - 90));
        this->addChild(waterSprite, 20);
    }

    int currentHoly = GameScene::getGlobalHolyWater();
    _holyLabel = Label::createWithTTF(std::to_string(currentHoly), "fonts/Marker Felt.ttf", 24);
    if (_holyLabel) {
        _holyLabel->setAnchorPoint(Vec2(0, 0.5f));
        if (waterSprite) _holyLabel->setPosition(waterSprite->getPosition() + Vec2(20, 0));
        else _holyLabel->setPosition(Vec2(visibleSize.width - 80, visibleSize.height - 90));
        this->addChild(_holyLabel, 20);
    }
    this->updateResourceUI();

    // 4. 初始化倒计时 Label 
    _timeLeft = 120.0f; // 2分钟
    _isGameOver = false;

    // 创建倒计时 Label
    _timeLabel = Label::createWithSystemFont("02:00", "Arial", 40);
    // 放在屏幕顶部正中间，稍微靠下一点，避免和难度文字重叠
    _timeLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 100));
    _timeLabel->setColor(Color3B::WHITE);
    _timeLabel->enableOutline(Color4B::BLACK, 2);
    this->addChild(_timeLabel, 100); // 加到场景里，update 才能用到它

    // 5. 注册监听
    auto goldListener = EventListenerCustom::create("LOOT_GOLD_EVENT", [this](EventCustom* event) {
        int* amount = static_cast<int*>(event->getUserData());
        if (amount) {
            GameScene::addGlobalResources(*amount, 0);
            this->updateResourceUI();
        }
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(goldListener, this);

    auto holyListener = EventListenerCustom::create("LOOT_HOLY_EVENT", [this](EventCustom* event) {
        int* amount = static_cast<int*>(event->getUserData());
        if (amount) {
            GameScene::addGlobalResources(0, *amount);
            this->updateResourceUI();
        }
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(holyListener, this);

    // 6. 游戏逻辑初始化
    generateLevel();
    initBattleUI();
    initTouchListener();

    this->scheduleUpdate(); // 开启 Update

    return true;
}

// 回放初始化 
bool FightScene::initForReplay(const ReplayData& data)
{
    if (!Scene::init()) return false;
    cocos2d::AudioEngine::stopAll();
    cocos2d::AudioEngine::play2d("fight_music.mp3", true, 0.5f);
    // 1. 数据重置
    SoldierManager::getInstance()->reset();
    BuildingManager::getInstance()->reset();
    _mySoldiers.clear();
    _enemyBuildings.clear();
    memset(mapGrid, 0, sizeof(mapGrid));

    // 2. 载入回放数据
    _isReplayMode = true;
    _replaySource = data;
    _replayActionIndex = 0;
    _difficulty = data.difficulty;

    // 应用种子
    std::srand(_replaySource.seed);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 3. 背景 (复制)
    auto bg = Sprite::create("GrassBackground.png");
    if (bg) {
        bg->setAnchorPoint(Vec2::ZERO);
        bg->setPosition(origin);
        float scaleX = visibleSize.width / bg->getContentSize().width;
        float scaleY = visibleSize.height / bg->getContentSize().height;
        bg->setScale(std::max(scaleX, scaleY));
        this->addChild(bg, 0);
    }

    // 4. 回放提示文字
    auto replayLabel = Label::createWithSystemFont("--- REPLAY MODE ---", "Arial", 48);
    replayLabel->setPosition(visibleSize.width / 2, visibleSize.height - 50);
    replayLabel->setColor(Color3B::GREEN);
    replayLabel->enableOutline(Color4B::BLACK, 3);
    this->addChild(replayLabel, 100);

    // 5. 初始化时间
    _timeLeft = 120.0f;
    _isGameOver = false;
    _timeLabel = Label::createWithSystemFont("02:00", "Arial", 40);
    _timeLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 100));
    _timeLabel->enableOutline(Color4B::BLACK, 2);
    this->addChild(_timeLabel, 100);

    // 6. 生成关卡 
    generateLevel();

    this->scheduleUpdate();
    return true;
}

// 每帧更新逻辑
void FightScene::update(float dt)
{
    // 如果游戏结束了，就不再执行逻辑
    if (_isGameOver) return;

    // 1. 倒计时逻辑
    _timeLeft -= dt;
    if (_timeLeft < 0) _timeLeft = 0;

    // 更新 Label 显示 (格式 MM:SS)
    int minutes = static_cast<int>(_timeLeft / 60);
    int seconds = static_cast<int>(_timeLeft) % 60;
    _timeLabel->setString(cocos2d::StringUtils::format("%02d:%02d", minutes, seconds));

    // 回放自动操作逻辑
    if (_isReplayMode)
    {
        float timePassed = 120.0f - _timeLeft;
        // 检查是否有操作需要执行
        while (_replayActionIndex < _replaySource.actions.size())
        {
            auto& action = _replaySource.actions[_replayActionIndex];
            if (timePassed >= action.time)
            {
                // 时间到了，执行操作
                executeDeploySoldier(action.soldierType, Vec2(action.x, action.y));
                _replayActionIndex++;
            }
            else
            {
                // 时间没到，跳出循环等待
                break;
            }
        }
    }

    // 2. 清理逻辑 (士兵 + 建筑)

    // A. 清理死掉的士兵
    for (int i = _mySoldiers.size() - 1; i >= 0; i--)
    {
        Soldier* s = _mySoldiers.at(i);
        if (!s || s->getParent() == nullptr || s->getHP() <= 0)
        {
            // 确保从 vector 移除
            _mySoldiers.erase(i);
        }
    }

    // B. 清理被摧毁的建筑
    // 从 _enemyBuildings 列表中移除它，否则无法判断胜利。
    for (int i = _enemyBuildings.size() - 1; i >= 0; i--)
    {
        Building* b = _enemyBuildings.at(i);
        // 如果建筑已经被移出场景(被销毁)，或者血量<=0
        if (b->getParent() == nullptr || b->getHP() <= 0)
        {
            _enemyBuildings.erase(i);
        }
    }
    for (int i = _bombs.size() - 1; i >= 0; i--)
    {
        auto boom = _bombs.at(i);

        // 阶段 A: 如果已经被触发了，就进行倒计时
        if (boom->isTriggered)
        {
            // 1. 倒计时
            boom->delayTimer -= dt;

          

            // 3. 时间到了
            if (boom->delayTimer <= 0)
            {
                CCLOG("Boom exploded after delay!");

                // --- 播放特效 ---
                boom->playExplodeEffect();

                // --- 造成伤害 ---
                float damageRange = 200.0f;
                for (auto target : _mySoldiers)
                {
                    if (target && target->getHP() > 0)
                    {
                        float dist = boom->getPosition().distance(target->getPosition());
                        if (dist < damageRange)
                        {
                            target->takeDamage(boom->getDamage());
                        }
                    }
                }

                // --- 移除炸弹 ---
                _bombs.erase(i);
            }

            // 如果正在倒计时，就跳过下面的“碰撞检测”，处理下一个炸弹
            continue;
        }

        
        // 阶段 B: 如果还没触发，检测有没有人踩到
        bool collisionHappened = false;
        float boomX = boom->getPositionX();
        float boomY = boom->getPositionY();

        for (auto soldier : _mySoldiers)
        {
            if (soldier && soldier->getHP() > 0)
            {
                float solX = soldier->getPositionX();
                float solY = soldier->getPositionY();

                // 矩形检测 (128x128范围)
                if (std::abs(boomX - solX) <= 128.0f &&
                    std::abs(boomY - solY) <= 128.0f)
                {
                    collisionHappened = true;
                    break;
                }
            }
        }

        // 如果有人踩到了
        if (collisionHappened)
        {
            CCLOG("Bomb Triggered! Waiting 0.5s...");

            // 1. 标记为已触发，开始倒计时
            boom->isTriggered = true;
            boom->delayTimer = 0.5f; // 重置倒计时为 0.5 秒

           
            // 把透明度设回 255，或者你可以换一张“红色的雷”的图片
            boom->setOpacity(255);
            boom->setVisible(true);
        }
    }
    for (auto building : _enemyBuildings)
    {
        Cannon* cannon = dynamic_cast<Cannon*>(building);
        if (cannon)
        {
            Soldier* target = nullptr;
            float minDistance = cannon->getAttackRange();

            for (auto soldier : _mySoldiers)
            {
                float dist = cannon->getPosition().distance(soldier->getPosition());
                if (dist < minDistance)
                {
                    minDistance = dist;
                    target = soldier;
                }
            }
            if (target) cannon->fireAt(target);
        }
    }
    // 4. 检查胜负
    checkGameStatus();
}

// 实现索敌逻辑
Building* FightScene::getPriorityTarget(Vec2 soldierPos)
{
    Building* bestTarget = nullptr;
    float minDistance = 99999.0f;
    int highestPriority = -1; // 优先级：3=防御塔, 2=大本营, 1=其他

    // 遍历所有敌方建筑
    for (auto building : _enemyBuildings)
    {
        if (!building || building->getParent() == nullptr) continue;

        // 1. 确定当前建筑的优先级
        int priority = 1;

        if (dynamic_cast<ArrowTower*>(building))
        {
            priority = 3; // 最高优先级：先拆塔
        }
        // 加农炮也是最高优先级
        else if (dynamic_cast<Cannon*>(building))
        {
            priority = 3;
        }
        else if (dynamic_cast<TownHall*>(building))
        {
            priority = 2; // 次高：大本营
        }

        // 2. 计算距离
        float dist = soldierPos.distance(building->getPosition());

        // 3. 比较逻辑：
        // 情况A: 发现了更高优先级的建筑 -> 直接锁定，不管距离
        if (priority > highestPriority)
        {
            highestPriority = priority;
            bestTarget = building;
            minDistance = dist;
        }
        // 情况B: 优先级相同 -> 选更近的
        else if (priority == highestPriority)
        {
            if (dist < minDistance)
            {
                bestTarget = building;
                minDistance = dist;
            }
        }
    }

    return bestTarget;
}

void FightScene::generateLevel()
{
    // 1. 初始化
    memset(mapGrid, 0, sizeof(mapGrid));
    _enemyBuildings.clear();

    // 2. 难度设定 (原有逻辑 + 新增资源建筑数量)
    int targetLevel = 1;
    int arrowTowerCount = 0;
    int cannonCount = 0;

    // 定义要生成的资源建筑数量
    int goldStageCount = 0;
    int elixirTankCount = 0;
    int boomcount = 0;
    if (_difficulty == 1)
    {
        targetLevel = 1; arrowTowerCount = 0; cannonCount = 1;
        goldStageCount = 1; elixirTankCount = 1; boomcount = 2; // 简单：各1个
    }
    else if (_difficulty == 2)
    {
        targetLevel = 2; arrowTowerCount = 0; cannonCount = 2;
        goldStageCount = 1; elixirTankCount = 1; boomcount = 4; // 中等：各2个
    }
    else if (_difficulty == 3)
    {
        targetLevel = 3; arrowTowerCount = 0; cannonCount = 3;
        goldStageCount = 1; elixirTankCount = 1; boomcount = 6; // 困难：各3个
    }

    // 第一步：放置大本营 (TownHall 3x3)
    auto townHall = TownHall::create();
    setBuildingLevel(townHall, targetLevel);

    // 计算居中坐标
    int thCol = 13;
    int thRow = 6;

    markArea(thCol, thRow, 3, 3);
    townHall->setPosition(getPositionForGrid(thCol, thRow, 3, 3));
    this->addChild(townHall, 10);
    _enemyBuildings.pushBack(townHall);

    // 第二步：准备建筑列表 -- 将所有要生成的建筑都放进这个列表
    std::vector<Building*> pendingBuildings;

    // 2.1 原有的建筑
    pendingBuildings.push_back(MilitaryCamp::create());

    auto water = WaterCollection::create();
    water->setEnemyState(true);
    pendingBuildings.push_back(water);

    auto coin = CoinCollection::create();
    coin->setEnemyState(true);
    pendingBuildings.push_back(coin);

    // 2.2 防御塔和加农炮
    for (int i = 0; i < arrowTowerCount; i++)
    {
        pendingBuildings.push_back(ArrowTower::create());
    }
    for (int i = 0; i < cannonCount; i++)
    {
        pendingBuildings.push_back(Cannon::create());
    }

    // 2.3 金库和水罐
    for (int i = 0; i < goldStageCount; i++)
    {
        auto gs = GoldStage::create();
        gs->updateVisuals(5000, 5000); // 设为满资源状态
        pendingBuildings.push_back(gs);
    }
    for (int i = 0; i < elixirTankCount; i++)
    {
        auto et = ElixirTank::create();
        et->updateVisuals(5000, 5000); // 设为满资源状态
        pendingBuildings.push_back(et);
    }

    // 第三步：生成候选坐标 
    struct GridPoint
    {
        int x, y;
        float distanceScore;
    };
    std::vector<GridPoint> validSpots;

    float centerX = thCol + 1.5f; // 14.5
    float centerY = thRow + 1.5f; // 7.5

    for (int x = 1; x <= 28; x++)
    {
        for (int y = 1; y <= 14; y++)
        {

            // 排除掉已经被大本营占用的区域
            if (x >= thCol && x < thCol + 3 && y >= thRow && y < thRow + 3) continue;

            float dx = x - centerX;
            float dy = y - centerY;
            float dist = sqrt(dx * dx + dy * dy);

            float randomNoise = (static_cast<float>(rand()) / RAND_MAX) * 3.0f;

            validSpots.push_back({ x, y, dist + randomNoise });
        }
    }

    // 按距离排序
    std::sort(validSpots.begin(), validSpots.end(), [](const GridPoint& a, const GridPoint& b)
        {
            return a.distanceScore < b.distanceScore;
        });

    // 第四步：放置 (循环放置 pendingBuildings 里的所有建筑)
    for (auto b : pendingBuildings)
    {
        setBuildingLevel(b, targetLevel);
        bool placed = false;

        for (const auto& spot : validSpots)
        {
            // 其他建筑都是 2x2
            // 这里的 isAreaFree 会自动处理 GoldStage 和 ElixirTank 的位置判断
            if (isAreaFree(spot.x, spot.y, 2, 2))
            {
                markArea(spot.x, spot.y, 2, 2);
                b->setPosition(getPositionForGrid(spot.x, spot.y, 2, 2));

                this->addChild(b, 10);
                _enemyBuildings.pushBack(b);
                placed = true;
                break;
            }
        }

        if (!placed)
        {
            CCLOG("Warning: No space for building nearby center!");
        }
    }
    for (int i = 0; i < boomcount; i++)
    {
        auto boom = Boom::create();
        boom->setToFightMode(); // 隐形模式

        bool placed = false;
        int attempts = 0;

        // 我们限制炸弹生成在 X: 8~20, Y: 3~11 这个核心区域
        // 这样炸弹会集中在建筑群周围，而不是地图边缘
        int minX = 8;  int maxX = 20;
        int minY = 3;  int maxY = 11;

        while (!placed && attempts < 50)
        {
            // 在防御圈内随机
            int tx = minX + std::rand() % (maxX - minX + 1);
            int ty = minY + std::rand() % (maxY - minY + 1);

            // 检查位置是否空闲 (炸弹占地 1x1)
            if (isAreaFree(tx, ty, 1, 1))
            {
                markArea(tx, ty, 1, 1);
                boom->setPosition(getPositionForGrid(tx, ty, 1, 1));

                this->addChild(boom, 5);
                _bombs.pushBack(boom); // 加入独立列表

                placed = true;
            }
            attempts++;
        }

        if (!placed)
        {
            CCLOG("Warning: Could not place bomb nearby center!");
        }
    }
}
// 检查区域是否空闲 (包含 1 格子的安全间距)
bool FightScene::isAreaFree(int gridX, int gridY, int width, int height)
{
    // 1. 基础边界检查：建筑本体绝对不能超出地图
    // 地图大小: 30 x 16
    if (gridX < 0 || gridX + width > 30 || gridY < 0 || gridY + height > 16)
        return false;

    // 2. 扩充检查范围：检查本体 + 周围一圈 (padding)
    // 比如建筑是 2x2，我们实际检查 4x4 的范围
    int startX = gridX - 1;
    int endX = gridX + width + 1; // 注意这里是 +1，因为循环是 < endX
    int startY = gridY - 1;
    int endY = gridY + height + 1;

    for (int x = startX; x < endX; x++)
    {
        for (int y = startY; y < endY; y++)
        {

            // 如果检查点超出了地图边界(比如 x=-1)，我们认为那是“空”的（边缘可以走或者作为间隔），
            // 所以只关心在地图范围内的格子是否被占用。
            if (x >= 0 && x < 30 && y >= 0 && y < 16)
            {
                if (mapGrid[x][y])
                {
                    // 只要发现任何一个格子被占用，就说明太挤了
                    return false;
                }
            }
        }
    }

    return true;
}

// 标记网格区域为占用
void FightScene::markArea(int gridX, int gridY, int width, int height)
{
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int targetX = gridX + x;
            int targetY = gridY + y;

            // 检查边界
            if (targetX >= 0 && targetX < 30 && targetY >= 0 && targetY < 16)
            {
                mapGrid[targetX][targetY] = true;
            }
        }
    }
}

// 计算像素坐标 (吸附中心)
Vec2 FightScene::getPositionForGrid(int gridX, int gridY, int width, int height)
{
    // 公式：
    // X = 格子索引 * 64 + (占用格子数 * 64) / 2
    // 这样能保证锚点(0.5, 0.5)的图片正好居中显示在这些格子上

    float posX = gridX * TILE_SIZE + (width * TILE_SIZE) / 2.0f;
    float posY = gridY * TILE_SIZE + (height * TILE_SIZE) / 2.0f;

    return Vec2(posX, posY);
}

// 辅助：升级建筑 
void FightScene::setBuildingLevel(Building* building, int targetLevel)
{
    if (!building) return;
    int upgradesNeeded = targetLevel - 1;
    for (int i = 0; i < upgradesNeeded; i++) {
        building->upgrade();
    }
}

// 胜负判定
void FightScene::checkGameStatus()
{
    // 1. 胜利判定：所有敌人建筑都被摧毁
    if (_enemyBuildings.empty())
    {
        showGameOver(true); // Victory
        return;
    }

    // 2. 失败判定 A：时间耗尽
    if (_timeLeft <= 0)
    {
        showGameOver(false); // Defeat
        return;
    }

    // 3. 失败判定 B：无兵可放 且 场上兵全死
    // 回放模式下不检查库存失败条件，只看场上兵
    if (_isReplayMode) {
        if (_mySoldiers.empty() && !_enemyBuildings.empty() && _replayActionIndex >= _replaySource.actions.size()) {
            showGameOver(false);
            return;
        }
        return;
    }

    // 检查是否有库存
    bool hasReserves = false;
    for (int i = 1; i <= 4; i++) {
        if (GameScene::getGlobalSoldierCount(i) > 0) {
            hasReserves = true;
            break;
        }
    }

    // 如果 (没库存) AND (场上没活兵) AND (还有敌人建筑) -> 输
    if (!hasReserves && _mySoldiers.empty() && !_enemyBuildings.empty())
    {
        showGameOver(false); // Defeat
        return;
    }
}

void FightScene::showGameOver(bool isWin)
{
    _isGameOver = true; // 锁定状态，防止update继续跑

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 1. 创建结果图片
    std::string imgName = isWin ? "Victory.png" : "Defeat.png";
    auto resultSprite = Sprite::create(imgName);
    if(isWin)
    {
        cocos2d::AudioEngine::stopAll(); // 停掉激情的战斗BGM
        cocos2d::AudioEngine::play2d("win.mp3", false, 1.0f); // 播放一次胜利音效
    }
    else
    {
        cocos2d::AudioEngine::stopAll(); // 停掉激情的战斗BGM
        cocos2d::AudioEngine::play2d("battle_lost.mp3", false, 1.0f); // 播放一次失败音效
    }
    if (resultSprite)
    {
        resultSprite->setPosition(visibleSize.width / 2, visibleSize.height / 2);
        resultSprite->setScale(0.1f); // 初始很小，做弹窗动画
        this->addChild(resultSprite, 200); // 放在最上层

        // 动画：弹出来
        auto scaleTo = ScaleTo::create(0.5f, 1.0f);
        auto ease = EaseBackOut::create(scaleTo);
        resultSprite->runAction(ease);
    }
    else
    {
        // 如果没图，用文字代替
        auto label = Label::createWithSystemFont(isWin ? "VICTORY!" : "DEFEAT...", "Arial", 80);
        label->setPosition(visibleSize.width / 2, visibleSize.height / 2);
        label->setColor(isWin ? Color3B::YELLOW : Color3B::RED);
        label->enableOutline(Color4B::BLACK, 4);
        this->addChild(label, 200);
    }

    // 2. 停止所有战斗逻辑
    this->unscheduleUpdate();

    // 3. 停止所有正在进行的动作 (让士兵和塔停下来)
    // 遍历所有子节点暂停动作
    for (auto child : this->getChildren()) {

        // 如果这个节点是我们标记的“退出菜单”，就跳过，不暂停
        if (child->getTag() == 9999) continue;

        child->pause();
    }

    // 添加回放/退出按钮菜单
    Menu* menu = nullptr;

    if (!_isReplayMode)
    {
        // 正常结束：显示“观看回放”和“退出”
        auto replayLabel = Label::createWithSystemFont("Watch Replay", "Arial", 40);
        replayLabel->setColor(Color3B::GREEN);
        replayLabel->enableOutline(Color4B::BLACK, 2);

        auto replayItem = MenuItemLabel::create(replayLabel, [=](Ref*) {
            // 切换到回放场景，传入刚才录制的数据
            auto scene = FightScene::createReplayScene(_currentRecord);
            Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
            });

        auto exitLabel = Label::createWithSystemFont("Exit", "Arial", 40);
        exitLabel->setColor(Color3B::RED);
        exitLabel->enableOutline(Color4B::BLACK, 2);

        auto exitItem = MenuItemLabel::create(exitLabel, [=](Ref*) {
            Director::getInstance()->popScene();
            });

        menu = Menu::create(replayItem, exitItem, nullptr);
        menu->alignItemsHorizontallyWithPadding(100);
    }
    else
    {
        // 回放结束：只显示“返回”
        auto backLabel = Label::createWithSystemFont("Back", "Arial", 40);
        backLabel->enableOutline(Color4B::BLACK, 2);

        auto backItem = MenuItemLabel::create(backLabel, [=](Ref*) {
            Director::getInstance()->popScene();
            });
        menu = Menu::create(backItem, nullptr);
    }

    if (menu) {
        menu->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 200);
        this->addChild(menu, 300);
    }

    CCLOG("Game Over: %s", isWin ? "Win" : "Lose");
}