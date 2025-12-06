#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "cocos2d.h"

class Building : public cocos2d::Sprite
{
public:
    virtual bool init() override;
    virtual void upgrade() = 0; // 纯虚函数，由子类实现

    int getLevel() const { return level; }
    void takeDamage(int dmg);   // 受伤
    void updateHPBar();         // 更新血条显示

protected:
    int level = 1;

    int maxHP = 100;
    int currentHP = 100;

    cocos2d::DrawNode* hpBar = nullptr;

    void addTouchListener(); // 点击事件
};

#endif
