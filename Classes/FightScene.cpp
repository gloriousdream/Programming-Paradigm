#include "FightScene.h"
#include "GameScene.h"
#include <string>
#include "TownHall.h"
#include "ArrowTower.h"
#include "MilitaryCamp.h"
#include "WaterCollection.h"
#include "CoinCollection.h"

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

bool FightScene::init()
{
    if (!Scene::init()) return false;
    return true;
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

    return true;
}

void FightScene::generateLevel()
{
    // 1. 初始化：清空网格和建筑列表
    memset(mapGrid, 0, sizeof(mapGrid));
    _enemyBuildings.clear();

    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 2. 确定难度参数
    int targetLevel = 1;
    int arrowTowerCount = 0;

    // 确保每种难度都有基础建筑，只调整等级和防御塔数量
    if (_difficulty == 1) {
        targetLevel = 1;
        arrowTowerCount = 1; // 简单：1个塔
    }
    else if (_difficulty == 2) {
        targetLevel = 2;
        arrowTowerCount = 2; // 中等：2个塔
    }
    else if (_difficulty == 3) {
        targetLevel = 3;
        arrowTowerCount = 2; // 困难：2个塔 (高等级)
    }

    // 第一步：放置大本营 (TownHall 3x3) - 绝对中心
    auto townHall = TownHall::create();
    setBuildingLevel(townHall, targetLevel);

    // 大本营网格坐标 (14, 10)
    int thCol = 14;
    int thRow = 10;

    // 标记占用并设置位置
    markArea(thCol, thRow, 3, 3);
    townHall->setPosition(getPositionForGrid(thCol, thRow, 3, 3));
    this->addChild(townHall, 10);
    _enemyBuildings.pushBack(townHall);

    // 第二步：准备要生成的建筑列表 (确保每种都有)
    std::vector<Building*> pendingBuildings;

    // 1. 兵营 (MilitaryCamp) - 必有
    pendingBuildings.push_back(MilitaryCamp::create());

    // 2. 圣水收集器 (WaterCollection) - 必有
    auto water = WaterCollection::create();
    water->setEnemyState(true); // 敌人模式：不生产
    pendingBuildings.push_back(water);

    // 3. 金矿 (CoinCollection) - 必有
    auto coin = CoinCollection::create();
    coin->setEnemyState(true); // 敌人模式：不生产
    pendingBuildings.push_back(coin);

    // 4. 箭塔 (ArrowTower) - 根据难度决定数量
    for (int i = 0; i < arrowTowerCount; i++) {
        pendingBuildings.push_back(ArrowTower::create());
    }

    // 第三步：生成“离中心最近”的候选坐标列表
    struct GridPoint {
        int x, y;
        float distanceScore; // 距离评分
    };
    std::vector<GridPoint> validSpots;

    // 大本营中心点 (格子坐标系)
    float centerX = thCol + 1.5f; // 3格宽，中心在 1.5
    float centerY = thRow + 1.5f;

    // 遍历地图上的位置 (留出边缘 buffer，不要太靠边)
    for (int x = 2; x <= 29; x++) {
        for (int y = 2; y <= 21; y++) {

            // 计算到大本营中心的距离
            float dx = x - centerX;
            float dy = y - centerY;
            float dist = sqrt(dx * dx + dy * dy);

            // 【关键】：距离 + 随机噪音
            // 如果只用 dist，每次生成的布局都一模一样。
            // 加一点随机数 (0~4)，让同心圆层的顺序被打乱，看起来更自然。
            float randomNoise = CCRANDOM_0_1() * 4.0f;

            validSpots.push_back({ x, y, dist + randomNoise });
        }
    }

    // 按距离评分从小到大排序 (离中心越近越前)
    std::sort(validSpots.begin(), validSpots.end(), [](const GridPoint& a, const GridPoint& b) {
        return a.distanceScore < b.distanceScore;
        });

    // 第四步：依次放置建筑 (吸附逻辑)
    for (auto b : pendingBuildings)
    {
        setBuildingLevel(b, targetLevel);

        bool placed = false;

        // 遍历排序后的坐标，找第一个能塞进去的地方
        for (const auto& spot : validSpots)
        {
            // 其他建筑都是 2x2
            if (isAreaFree(spot.x, spot.y, 2, 2))
            {
                // 找到位置
                markArea(spot.x, spot.y, 2, 2);
                b->setPosition(getPositionForGrid(spot.x, spot.y, 2, 2));

                this->addChild(b, 10);
                _enemyBuildings.pushBack(b);
                placed = true;
                break; // 放下了，就处理下一个建筑
            }
        }

        if (!placed) {
            CCLOG("Warning: No space for building nearby center!");
        }
    }
}

// 检查网格区域是否被占用
bool FightScene::isAreaFree(int gridX, int gridY, int width, int height)
{
    // 边界检查
    if (gridX < 0 || gridX + width > 32 || gridY < 0 || gridY + height > 24) return false;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (mapGrid[gridX + x][gridY + y]) {
                return false; // 只要有一个格子被占，就返回 false
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

// 辅助：升级建筑 (保持不变)
void FightScene::setBuildingLevel(Building* building, int targetLevel)
{
    if (!building) return;
    int upgradesNeeded = targetLevel - 1;
    for (int i = 0; i < upgradesNeeded; i++) {
        building->upgrade();
    }
}