#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "cocos2d.h"

// 抽象基类
class Building : public cocos2d::Sprite
{
public:
    virtual bool init() override;
    virtual void upgrade() = 0; // 纯虚函数，由子类实现
    int getHP() const { return currentHP; }
    int getMaxHP() const { return maxHP; }
    int getLevel() const { return level; }
    bool isDead() const { return _isDead; } // 获取死亡状态
    virtual void takeDamage(int dmg);   // 受伤
    void updateHPBar();         // 更新血条显示

    // 建造 / 升级 消耗（子类在 init 中赋值）
    int buildCostGold = 0;
    int buildCostHoly = 0;
    int upgradeCostGold = 0;
    int upgradeCostHoly = 0;

protected:
    int level = 1;
    int maxHP = 100;
    int currentHP = 100;

    bool _isDead = false; // 死亡标记

    cocos2d::DrawNode* hpBar = nullptr;

    void addTouchListener(); // 点击事件，发送 "BUILDING_CLICKED"
};

#endif
