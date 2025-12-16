#pragma once
#ifndef __GOLD_STAGE_H__
#define __GOLD_STAGE_H__

#include "Building.h"

class GoldStage : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(GoldStage);
    virtual void takeDamage(int damage) override;
    virtual void upgrade() override;
    // 根据当前金币和最大容量更新图片
    void updateVisuals(int currentGold, int maxGold);

private:
    int _visualLevel = -1; // 用于记录当前状态，避免重复刷新
};

#endif