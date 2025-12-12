#ifndef __WATER_COLLECTION_H__
#define __WATER_COLLECTION_H__

#include "Building.h"

class WaterCollection : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(WaterCollection);
    virtual void upgrade() override;

    // 设置为敌人状态（停止生产）
    void setEnemyState(bool isEnemy);

private:
    cocos2d::Sprite* rewardIcon = nullptr;

    // 当前累积的资源数量
    int currentStorage = 0;
    // 存储上限 (随等级提升)
    int maxStorage = 0;

    // 定时生产函数
    void produceResource(float dt);

    // 处理收集点击
    void onCollect();
};

#endif