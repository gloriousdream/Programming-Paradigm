#include "FightScene.h"
#include "GameScene.h"
#include <string>
#include "TownHall.h"
#include "ArrowTower.h"
#include "MilitaryCamp.h"
#include "WaterCollection.h"
#include "CoinCollection.h"
#include "SoldierManager.h"
#include "Cannon.h"

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
        {"juren_select.png",     2, "Giant"},
        {"gongjianshou_select.png",    3, "Archer"},
        {"boom_select.png",    4, "Bomber"}
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

        // 在图标下方加个名字
        auto nameLabel = Label::createWithSystemFont(opt.name, "Arial", 20);
        nameLabel->setPosition(Vec2(item->getContentSize().width / 2, -20));
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

// 【新增】添加士兵的接口
void FightScene::addSoldier(Soldier* soldier)
{
    if (!soldier) return;

    // 1. 加到场景显示 (Z轴设为15，盖在地面上)
    this->addChild(soldier, 15);

    // 2. 加到逻辑列表 (给加农炮索敌用)
    _mySoldiers.pushBack(soldier);
}

void FightScene::onMapClick(Vec2 pos)
{
    if (_selectedSoldierType == 0) return;

    auto soldier = SoldierManager::getInstance()->createSoldier(_selectedSoldierType, pos);
    if (soldier)
    {
        // 【关键修改】使用 addSoldier 替代原来的 addChild
        // 这样加农炮才能识别到这个士兵
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
    if (!Scene::init()) return false;

    _difficulty = difficulty;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 1. 加载背景
    auto bg = Sprite::create("GrassBackground.png");
    if (bg) {
        bg->setAnchorPoint(Vec2::ZERO);
        bg->setPosition(origin);
        this->addChild(bg, 0);
    }

    // 2. 显示难度文字
    std::string diffText = "";
    if (_difficulty == 1) diffText = "Mode: EASY";
    else if (_difficulty == 2) diffText = "Mode: MIDDLE";
    else if (_difficulty == 3) diffText = "Mode: HARD";

    auto label = Label::createWithSystemFont(diffText, "Arial", 32);
    label->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 50));
    label->setColor(Color3B::RED);
    this->addChild(label, 100);

    // 3. 返回按钮 (popScene)
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
    this->addChild(menu, 100);

    // 4. 生成敌人基地
    generateLevel();
    initBattleUI();
    initTouchListener(); //监听草地点击

    // 【新增】开启帧更新，让加农炮能攻击
    this->scheduleUpdate();

    return true;
}

// 【新增】每帧更新逻辑
void FightScene::update(float dt)
{
    // 1. 清理已死亡的士兵 (倒序遍历)
    for (int i = _mySoldiers.size() - 1; i >= 0; i--)
    {
        Soldier* s = _mySoldiers.at(i);
        // 如果士兵已经被移出父节点 或 血量归零
        if (s->getParent() == nullptr || s->getHP() <= 0)
        {
            _mySoldiers.erase(i);
        }
    }

    // 2. 遍历敌方建筑，让防御塔攻击
    for (auto building : _enemyBuildings)
    {
        // 判断是否为加农炮
        Cannon* cannon = dynamic_cast<Cannon*>(building);
        if (cannon)
        {
            // 寻找最近的士兵
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

            // 如果找到目标，开火 (Cannon::fireAt 内部处理冷却和动画)
            if (target)
            {
                cannon->fireAt(target);
            }
        }
    }
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
        // 【新增】加农炮也是最高优先级
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

    // 2. 难度设定
    int targetLevel = 1;
    int arrowTowerCount = 0;
    int cannonCount = 0; // 【新增】加农炮数量

    if (_difficulty == 1) { targetLevel = 1; arrowTowerCount = 1; cannonCount = 0; }
    else if (_difficulty == 2) { targetLevel = 2; arrowTowerCount = 1; cannonCount = 1; }
    else if (_difficulty == 3) { targetLevel = 3; arrowTowerCount = 2; cannonCount = 1; }

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

    // 第二步：准备建筑列表
    std::vector<Building*> pendingBuildings;
    pendingBuildings.push_back(MilitaryCamp::create());

    auto water = WaterCollection::create();
    water->setEnemyState(true);
    pendingBuildings.push_back(water);

    auto coin = CoinCollection::create();
    coin->setEnemyState(true);
    pendingBuildings.push_back(coin);

    for (int i = 0; i < arrowTowerCount; i++) {
        pendingBuildings.push_back(ArrowTower::create());
    }

    // 【新增】将加农炮加入待生成列表
    for (int i = 0; i < cannonCount; i++) {
        pendingBuildings.push_back(Cannon::create());
    }

    // 第三步：生成候选坐标 
    struct GridPoint {
        int x, y;
        float distanceScore;
    };
    std::vector<GridPoint> validSpots;

    float centerX = thCol + 1.5f; // 14.5
    float centerY = thRow + 1.5f; // 7.5

    for (int x = 1; x <= 28; x++) {
        for (int y = 1; y <= 14; y++) {

            // 排除掉已经被大本营占用的区域
            if (x >= thCol && x < thCol + 3 && y >= thRow && y < thRow + 3) continue;

            float dx = x - centerX;
            float dy = y - centerY;
            float dist = sqrt(dx * dx + dy * dy);

            // 加上随机噪音
            float randomNoise = CCRANDOM_0_1() * 3.0f;

            validSpots.push_back({ x, y, dist + randomNoise });
        }
    }

    // 按距离排序
    std::sort(validSpots.begin(), validSpots.end(), [](const GridPoint& a, const GridPoint& b) {
        return a.distanceScore < b.distanceScore;
        });

    // 第四步：放置
    for (auto b : pendingBuildings)
    {
        setBuildingLevel(b, targetLevel);
        bool placed = false;

        for (const auto& spot : validSpots)
        {
            // 其他建筑都是 2x2
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

        if (!placed) {
            CCLOG("Warning: No space for building nearby center!");
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
            mapGrid[gridX + x][gridY + y] = true;
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