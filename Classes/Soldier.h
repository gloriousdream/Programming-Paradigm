#pragma once
#ifndef __SOLDIER_H__
#define __SOLDIER_H__

#include "cocos2d.h"

class Soldier : public cocos2d::Sprite
{
public:
    // 初始化
    virtual bool init() override;

    // [接口] 纯虚函数：每个兵种必须实现自己的移动逻辑
    virtual void actionWalk() = 0;

    // 设置初始巡逻中心点（通常是出生点）
    void setHomePosition(cocos2d::Vec2 pos);

protected:
    cocos2d::Vec2 homePosition;

    // 通用逻辑：让士兵在 homePosition 附近随机走动
    // radius: 巡逻半径
    void executeRandomWalk(float radius);
};

#endif