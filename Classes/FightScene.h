#ifndef __FIGHT_SCENE_H__
#define __FIGHT_SCENE_H__

#include "cocos2d.h"
#include "Building.h"
#include "Cannon.h"   
#include "Soldier.h"
#include "GoldStage.h"
#include "ElixirTank.h"
// 定义一个简单的结构体用于 A* 算法
struct AStarNode
{
    int x, y;
    int g, h;
    AStarNode* parent;

    AStarNode(int _x, int _y) : x(_x), y(_y), g(0), h(0), parent(nullptr) {}
    int getF() const { return g + h; }
};

class FightScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene(int difficulty);
    static FightScene* create(int difficulty);
    virtual bool initWithDifficulty(int difficulty);
    virtual bool init() override;

    // 每帧更新逻辑 (加农炮攻击核心)
    virtual void update(float dt) override;

    Building* getPriorityTarget(cocos2d::Vec2 soldierPos);

    // 核心寻路函数
    std::vector<cocos2d::Vec2> findPath(cocos2d::Vec2 startWorldPos, cocos2d::Vec2 targetWorldPos);

    // 辅助：找到建筑周围最近的可行走格子
    cocos2d::Vec2 findBestAttackPosition(cocos2d::Vec2 startPos, Building* targetBuilding);

    void addSoldier(Soldier* soldier);
    cocos2d::Vector<Soldier*>& getSoldiers() { return _mySoldiers; }

private:
    int _difficulty;
    cocos2d::Vector<Building*> _enemyBuildings;

    // 地图网格标记：true表示被占用，false表示空闲
    bool mapGrid[30][16];
    const int TILE_SIZE = 64; // 格子像素大小

    // 生成关卡核心逻辑
    void generateLevel();

    // 辅助：设置建筑等级
    void setBuildingLevel(Building* building, int targetLevel);

    // 检查某个区域是否空闲
    bool isAreaFree(int gridX, int gridY, int width, int height);
    cocos2d::Label* _goldLabel = nullptr;
    cocos2d::Label* _holyLabel = nullptr;
    void updateResourceUI();
    // 标记某个区域为占用
    void markArea(int gridX, int gridY, int width, int height);

    // 根据格子坐标算出屏幕像素坐标
    cocos2d::Vec2 getPositionForGrid(int gridX, int gridY, int width, int height);

    cocos2d::Layer* _deployMenuLayer = nullptr;

    // 当前选中的兵种类型 (0表示没选)
    int _selectedSoldierType = 0;

    // 初始化 UI
    void initBattleUI();
    // 初始化触摸监听
    void initTouchListener();

    // 弹窗控制
    void showDeployMenu();
    void hideDeployMenu();

    // 选中兵种后的回调
    void onSelectSoldier(int type);

    // 点击地图放兵
    void onMapClick(cocos2d::Vec2 pos);

    bool isValidGrid(int x, int y);
    bool isGridBlocked(int x, int y);

    cocos2d::Vector<Soldier*> _mySoldiers;
};

#endif // __FIGHT_SCENE_H__