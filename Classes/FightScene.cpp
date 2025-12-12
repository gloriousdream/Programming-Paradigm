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
    // 1. 初始化
    memset(mapGrid, 0, sizeof(mapGrid));
    _enemyBuildings.clear();

    // 2. 难度设定
    int targetLevel = 1;
    int arrowTowerCount = 0;
    if (_difficulty == 1) { targetLevel = 1; arrowTowerCount = 1; }
    else if (_difficulty == 2) { targetLevel = 2; arrowTowerCount = 2; }
    else if (_difficulty == 3) { targetLevel = 3; arrowTowerCount = 2; }

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

// 边界检查适配 
bool FightScene::isAreaFree(int gridX, int gridY, int width, int height)
{
    // 宽 30，高 16
    if (gridX < 0 || gridX + width > 30 || gridY < 0 || gridY + height > 16) return false;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            if (mapGrid[gridX + x][gridY + y]) {
                return false;
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