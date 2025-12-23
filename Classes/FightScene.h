#ifndef __FIGHT_SCENE_H__
#define __FIGHT_SCENE_H__
#include "Boom.h"
#include "cocos2d.h"
#include "Building.h"
#include "Cannon.h"
#include "Soldier.h"
#include "GoldStage.h"
#include "ElixirTank.h"

// 回放数据结构
struct ReplayActionData {
    float time;      // 时间点
    int soldierType; // 兵种
    float x, y;      // 坐标
};

struct ReplayData {
    unsigned int seed; // 随机种子 (核心)
    int difficulty;
    std::vector<ReplayActionData> actions; // 操作列表
};

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

    // 回放场景创建入口
    static FightScene* createReplayScene(const ReplayData& data);

    virtual bool initWithDifficulty(int difficulty);

    // 回放初始化
    virtual bool initForReplay(const ReplayData& data);

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
    cocos2d::Vector<Boom*> _bombs;
    // 存储地方建筑
    cocos2d::Vector<Building*> _enemyBuildings;
    // 存储我方士兵
    cocos2d::Vector<Soldier*> _mySoldiers;

    // 地图网格标记：true表示被占用，false表示空闲
    bool mapGrid[30][16];
    const int TILE_SIZE = 64; // 格子像素大小

    // 回放控制变量 
    bool _isReplayMode = false;
    ReplayData _currentRecord;    // 录制用
    ReplayData _replaySource;     // 回放用
    int _replayActionIndex = 0;   // 播放进度

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

    // 执行放兵逻辑 (与点击分离，供回放调用)
    void executeDeploySoldier(int soldierType, cocos2d::Vec2 pos);

    bool isValidGrid(int x, int y);
    bool isGridBlocked(int x, int y);

    float _timeLeft = 120.0f; // 剩余时间
    cocos2d::Label* _timeLabel = nullptr; // 倒计时显示 Label
    bool _isGameOver = false; // 游戏结束标记

    // 检查游戏胜负状态
    void checkGameStatus();

    // 显示结算画面
    void showGameOver(bool isWin);
};

#endif // __FIGHT_SCENE_H__