#include "Soldier.h"
#include "FightScene.h" 
#include "GameScene.h"

USING_NS_CC;

bool Soldier::init()
{
    if (!Sprite::init()) return false;
    this->schedule(CC_SCHEDULE_SELECTOR(Soldier::updateSoldierLogic));

    // 初始化默认活动区域
    auto visibleSize = Director::getInstance()->getVisibleSize();
    _moveArea = Rect(0, 0, visibleSize.width, visibleSize.height);

    return true;
}

void Soldier::setHomePosition(Vec2 pos) { this->homePosition = pos; }
void Soldier::setMoveArea(const Rect& area) { this->_moveArea = area; }

void Soldier::setTargetBuilding(Building* building)
{
    _targetBuilding = building;
    // 重置状态为 IDLE，确保下一帧 update 能检测到移动并触发 actionWalk
    _state = SoldierState::IDLE;
}

void Soldier::setPath(const std::vector<Vec2>& pathPoints)
{
    _path = pathPoints;
    _currentPathIndex = 0;
    // 【关键】同上，重置状态
    _state = SoldierState::IDLE;
}

void Soldier::updateSoldierLogic(float dt)
{
    // 1. 目标状态校验
    // 如果目标刚死，或者已经被移除
    if (_targetBuilding && (_targetBuilding->getParent() == nullptr))
    {
        _targetBuilding = nullptr;
        stopAllActions();
        _state = SoldierState::IDLE;
    }

    // 2. 如果没有目标，尝试寻找目标 (核心修复区域)
    if (!_targetBuilding)
    {
        auto scene = Director::getInstance()->getRunningScene();

        // 只有在【战斗场景】才需要自动索敌
        auto fightScene = dynamic_cast<FightScene*>(scene);
        if (fightScene)
        {
            auto newTarget = fightScene->getPriorityTarget(this->getPosition());

            if (newTarget)
            {
                // 找到了新目标 -> 设置目标，计算路径
                this->setTargetBuilding(newTarget);
                Vec2 attackPos = fightScene->findBestAttackPosition(this->getPosition(), newTarget);
                auto path = fightScene->findPath(this->getPosition(), attackPos);
                this->setPath(path);
            }
            else
            {
              
                // 在战斗场景中，如果 getPriorityTarget 返回空，说明全图都被推平了。
                // 此时必须强制 return，防止代码往下执行到“巡逻逻辑”。

                if (_state != SoldierState::IDLE)
                {
                    stopAllActions();
                    _state = SoldierState::IDLE;
                }
                return; // 这一行阻止了士兵去巡逻
            }
        }
        else
        {
            // 如果不是 FightScene (比如是在自家兵营 GameScene)，
            // 没有目标时，允许代码往下执行，进入“巡逻逻辑”。
        }
    }

    // 3. 战斗执行逻辑 (移动 / 攻击)
    if (_targetBuilding)
    {
        float distToTarget = this->getPosition().distance(_targetBuilding->getPosition());

        // A. 攻击判定
        if (distToTarget <= _attackRange)
        {
            if (_state != SoldierState::ATTACKING)
            {
                _state = SoldierState::ATTACKING;
                this->stopAllActions();
                this->actionAttack();
            }
            return;
        }

        // B. 沿路径移动
        if (!_path.empty() && _currentPathIndex < _path.size())
        {
            Vec2 currentWaypoint = _path[_currentPathIndex];

            if (this->getPosition().distance(currentWaypoint) < 5.0f)
            {
                _currentPathIndex++;
                return;
            }

            Vec2 direction = (currentWaypoint - this->getPosition()).getNormalized();

            // 拐弯检测
            float dot = _curMoveDir.dot(direction);
            bool directionChanged = (dot < 0.5f) || (_curMoveDir.x * direction.x < 0);
            _curMoveDir = direction;

            if (directionChanged && _state == SoldierState::WALKING)
            {
                this->actionWalk();
            }

            float speed = 80.0f;
            this->setPosition(this->getPosition() + direction * speed * dt);

            if (_state != SoldierState::WALKING)
            {
                _state = SoldierState::WALKING;
                this->actionWalk();
            }

            // 简单翻转
            if (direction.x < 0) this->setFlippedX(true);
            else this->setFlippedX(false);

            return;
        }
        else
        {
            // C. 容错直连移动
            Vec2 dir = (_targetBuilding->getPosition() - this->getPosition()).getNormalized();
            if (dir.length() > 0)
            {
                _curMoveDir = dir;
                this->setPosition(this->getPosition() + dir * 60.0f * dt);
                if (dir.x < 0) this->setFlippedX(true);
                else this->setFlippedX(false);

                if (_state != SoldierState::WALKING)
                {
                    _state = SoldierState::WALKING;
                    this->actionWalk();
                }
            }
            return;
        }
    }

   
    // 4. 巡逻模式 (仅在 非战斗场景 或 找不到FightScene时 执行)
    // 这里的逻辑现在只有在 GameScene (家园) 里才会触发
    updatePatrolLogic(dt);
}
void Soldier::updatePatrolLogic(float dt)
{
    if (_isPatrolMoving)
    {
        float dist = this->getPosition().distance(_patrolTarget);
        if (dist < 5.0f)
        {
            _isPatrolMoving = false;
            _patrolWaitTimer = 2.0f;
            stopAllActions();
            _state = SoldierState::IDLE;
        }
        else
        {
            Vec2 dir = (_patrolTarget - this->getPosition()).getNormalized();
            _curMoveDir = dir;
            this->setPosition(this->getPosition() + dir * 50.0f * dt);

            if (dir.x < 0) this->setFlippedX(true);
            else this->setFlippedX(false);

            if (_state != SoldierState::WALKING)
            {
                _state = SoldierState::WALKING;
                this->actionWalk();
            }
        }
    }
    else
    {
        _patrolWaitTimer -= dt;
        if (_patrolWaitTimer <= 0)
        {
            float randX = _moveArea.origin.x + CCRANDOM_0_1() * _moveArea.size.width;
            float randY = _moveArea.origin.y + CCRANDOM_0_1() * _moveArea.size.height;
            _patrolTarget = Vec2(randX, randY);
            _isPatrolMoving = true;
        }
    }
}