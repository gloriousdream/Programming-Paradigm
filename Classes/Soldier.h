#pragma once
#ifndef __SOLDIER_H__
#define __SOLDIER_H__

#include "cocos2d.h"

class Soldier : public cocos2d::Sprite
{
public:
    virtual bool init() override;

    // 纯虚函数：每个兵种必须实现自己的移动逻辑
    virtual void actionWalk() = 0;

    //  设置活动区域
    void setMoveArea(const cocos2d::Rect& area);

    // 设置初始家位置
    void setHomePosition(cocos2d::Vec2 pos);

protected:
    cocos2d::Vec2 homePosition;

    //  存储活动区域
    cocos2d::Rect _moveArea;

    //  辅助函数：获取区域内的一个随机点
    cocos2d::Vec2 getRandomPointInArea();
};

#endif