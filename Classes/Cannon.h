#ifndef __CANNON_H__
#define __CANNON_H__

#include "Building.h"
#include "cocos2d.h"

// 前置声明
class Soldier;

class Cannon : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(Cannon);

    virtual void upgrade() override;
    virtual void update(float dt) override;

    // 攻击逻辑
    bool fireAt(Soldier* target);

    // 获取属性
    float getAttackRange() const { return attackRange; }
    int getAttackDamage() const { return attackDamage; }

private:
    int attackDamage;
    float attackRange;
    float attackRate;
    float _cooldownTimer;

    cocos2d::Sprite* _barrel; // 炮管Sprite

    // 转向逻辑
    void updateBarrelDirection(cocos2d::Vec2 targetPos);
};

#endif // __CANNON_H__