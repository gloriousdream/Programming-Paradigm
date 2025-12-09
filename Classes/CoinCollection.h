#ifndef __COIN_COLLECTION_H__
#define __COIN_COLLECTION_H__

#include "Building.h"

class CoinCollection : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(CoinCollection);
    virtual void upgrade() override;

private:
    // 奖励图标 (Coin.png)
    cocos2d::Sprite* rewardIcon = nullptr;

    // 是否可以收集
    bool isReadyToCollect = false;

    // 生产资源的回调函数
    void produceResource(float dt);

    // 处理收集点击
    void onCollect();
};

#endif