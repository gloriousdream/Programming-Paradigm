#pragma once
#ifndef __BARBARIAN_H__
#define __BARBARIAN_H__

#include "Soldier.h"

class Barbarian : public Soldier
{
public:
    virtual bool init() override;
    CREATE_FUNC(Barbarian);

    // 实现基类的接口
    virtual void actionWalk() override;
private:
    // 动作Tag，用于停止动画
    const int TAG_WALK_ACTION = 999;

    // 辅助函数：根据前缀(如 "barbarian_side_walk") 生成动画动作
    cocos2d::Animate* createAnimate(const std::string& prefix, int frameCount);
};

#endif