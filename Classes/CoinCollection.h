#ifndef __COIN_COLLECTION_H__
#define __COIN_COLLECTION_H__

#include "Building.h"

class CoinCollection : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(CoinCollection);
    virtual void upgrade() override;
    virtual std::string getNextLevelTextureName() override;

    void setEnemyState(bool isEnemy);

private:
    cocos2d::Sprite* rewardIcon = nullptr;

    int currentStorage = 0; // 当前累积
    int maxStorage = 0;     // 上限

    void produceResource(float dt);
    void onCollect();
};

#endif