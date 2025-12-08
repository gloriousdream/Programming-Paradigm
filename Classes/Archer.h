#pragma once
#ifndef __ARCHER_H__
#define __ARCHER_H__

#include "Soldier.h"

class Archer : public Soldier
{
public:
    virtual bool init() override;
    CREATE_FUNC(Archer);

    // 实现基类的走路接口
    virtual void actionWalk() override;

private:
    // 动作 Tag，用于停止动画
    const int TAG_WALK_ACTION = 888;

    // 辅助函数：快速创建动画动作
    cocos2d::Animate* createAnimate(const std::string& prefix, int frameCount);
};

#endif