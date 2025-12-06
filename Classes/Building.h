#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "cocos2d.h"

class Building : public cocos2d::Sprite
{
public:
    virtual bool init() override;
    virtual void upgrade() = 0; // 纯虚函数，由子类实现
    int getLevel() const { return level; }

protected:
    int level = 1;
    void addTouchListener(); // 点击事件
};

#endif
