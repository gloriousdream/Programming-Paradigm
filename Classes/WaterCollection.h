#ifndef __WATER_COLLECTION_H__
#define __WATER_COLLECTION_H__

#include "Building.h"

class WaterCollection : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(WaterCollection);
    virtual void upgrade() override;

private:
    // 奖励图标 (Water.png)
    cocos2d::Sprite* rewardIcon = nullptr;

    // 是否可以收集
    bool isReadyToCollect = false;

    // 生产资源的回调函数 (10秒到期后调用)
    void produceResource(float dt);

    // 处理收集点击
    void onCollect();
};

#endif