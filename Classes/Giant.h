#pragma once
#ifndef __GIANT_H__
#define __GIANT_H__

#include "Soldier.h"

class Giant : public Soldier
{
public:
    virtual bool init() override;
    CREATE_FUNC(Giant);

    virtual void actionWalk() override;

private:
    const int TAG_WALK_ACTION = 777; // 巨人的专属Tag

    // 辅助函数
    cocos2d::Animate* createAnimate(const std::string& prefix, int frameCount);
};

#endif