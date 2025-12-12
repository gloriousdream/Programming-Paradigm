#ifndef __FIGHT_SCENE_H__
#define __FIGHT_SCENE_H__

#include "cocos2d.h"
#include "Building.h"

class FightScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene(int difficulty);
    static FightScene* create(int difficulty);
    virtual bool initWithDifficulty(int difficulty);
    virtual bool init() override;

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

    // 检查某个区域是否空闲 (gridX, gridY 是左下角格子索引, width/height 是占几格)
    bool isAreaFree(int gridX, int gridY, int width, int height);

    // 标记某个区域为占用
    void markArea(int gridX, int gridY, int width, int height);

    // 根据格子坐标算出屏幕像素坐标 (吸附到中心)
    cocos2d::Vec2 getPositionForGrid(int gridX, int gridY, int width, int height);
};

#endif // __FIGHT_SCENE_H__