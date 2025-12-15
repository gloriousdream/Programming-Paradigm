#pragma once
#ifndef __SOLDIER_H__
#define __SOLDIER_H__

#include "cocos2d.h"
#include "Building.h"

enum class SoldierState
{
    IDLE,
    WALKING,
    ATTACKING
};

class Soldier : public cocos2d::Sprite
{
public:
    virtual bool init() override;

    // 纯虚函数：每个兵种必须实现自己的动作逻辑
    virtual void actionWalk() = 0;
    virtual void actionAttack() = 0;

    void setHomePosition(cocos2d::Vec2 pos);
    void setMoveArea(const cocos2d::Rect& area);
    void setTargetBuilding(Building* building);
    void setPath(const std::vector<cocos2d::Vec2>& pathPoints);

    // 获取当前瞬间的移动方向（供子类 Barbarian/Archer 等决定播放哪个面的动画）
    cocos2d::Vec2 getCurrentDirection() const { return _curMoveDir; }

    void setHP(int hp) { _hp = hp; _maxHp = hp; }
    int getHP() const { return _hp; }
    void takeDamage(int dmg); // 声明受伤函数

protected:
    cocos2d::Vec2 homePosition;
    cocos2d::Rect _moveArea;
    Building* _targetBuilding = nullptr;
    SoldierState _state = SoldierState::IDLE;
    float _attackRange = 80.0f; // 默认近战距离，弓箭手会覆盖它

    // 路径数据
    std::vector<cocos2d::Vec2> _path;
    int _currentPathIndex = 0;

    // 记录当前的移动向量，修复寻路时的朝向问题
    cocos2d::Vec2 _curMoveDir = cocos2d::Vec2(1, 0);

    // 巡逻相关变量
    cocos2d::Vec2 _patrolTarget;
    bool _isPatrolMoving = false;
    float _patrolWaitTimer = 0.0f;

    void updateSoldierLogic(float dt);
    void updatePatrolLogic(float dt);

    int _hp = 0;
    int _maxHp = 0;
};

#endif