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

    // 创建血条
    hpBar = DrawNode::create();
    this->addChild(hpBar, 10); // 层级10，确保显示在最上层
    updateHPBar();

    return true;
}

// 参照 Building::updateHPBar 实现
void Soldier::updateHPBar()
{
    if (!hpBar) return;
    hpBar->clear();

    // 保护：如果图片还没加载，尺寸可能为0
    Size size = getContentSize();
    if (size.width <= 0) size = Size(64, 64); // 默认防崩溃尺寸

    // 1. 计算尺寸和位置 
    float width = size.width * 0.8f;  // 血条宽度占 80%
    float height = 6;                 // 高度 6
    float x = (size.width - width) / 2;
    float y = size.height + 5;        // 显示在上方 5 像素

    // 2. 计算比例
    float hpPercent = (float)currentHP / (float)maxHP;
    if (hpPercent < 0) hpPercent = 0;
    if (hpPercent > 1) hpPercent = 1;

    // 3. 设置颜色
    Color4F backColor(0.3f, 0.3f, 0.3f, 1.0f); // 灰色背景 
    Color4F fillColor(1.0f, 0.0f, 0.0f, 1.0f); // 【红色】前景 

    // 4. 绘制
    // 背景灰色矩形
    hpBar->drawSolidRect(Vec2(x, y), Vec2(x + width, y + height), backColor);
    // 当前血量红色矩形
    hpBar->drawSolidRect(Vec2(x, y), Vec2(x + width * hpPercent, y + height), fillColor);
}

void Soldier::takeDamage(int dmg)
{
    currentHP -= dmg;
    if (currentHP < 0) currentHP = 0;

    // 刷新血条
    updateHPBar();

    // 死亡逻辑
    if (currentHP <= 0)
    {
        // 停止动作
        stopAllActions();
        // 从父节点移除
        this->removeFromParentAndCleanup(true);
    }
}

void Soldier::setHomePosition(Vec2 pos) { this->homePosition = pos; }
void Soldier::setMoveArea(const Rect& area) { this->_moveArea = area; }

void Soldier::setTargetBuilding(Building* building)
{
    _targetBuilding = building;
    _state = SoldierState::IDLE;
}

void Soldier::setPath(const std::vector<Vec2>& pathPoints)
{
    _path = pathPoints;
    _currentPathIndex = 0;
    _state = SoldierState::IDLE;
}

void Soldier::updateSoldierLogic(float dt)
{
    // 1. 目标状态校验
    if (_targetBuilding)
    {
        // 如果目标被标记为死亡或者真的不在场景里了
        if (_targetBuilding->isDead())
        {
            _targetBuilding = nullptr; // 丢弃无效指针
            stopAllActions();
            _state = SoldierState::IDLE;
        }
    }

    // 2. 如果没有目标，尝试寻找目标
    if (!_targetBuilding)
    {
        auto scene = Director::getInstance()->getRunningScene();
        auto fightScene = dynamic_cast<FightScene*>(scene);
        if (fightScene)
        {
            auto newTarget = fightScene->getPriorityTarget(this->getPosition());

            if (newTarget)
            {
                this->setTargetBuilding(newTarget);
                Vec2 attackPos = fightScene->findBestAttackPosition(this->getPosition(), newTarget);
                auto path = fightScene->findPath(this->getPosition(), attackPos);
                this->setPath(path);
            }
            else
            {
                // 全图推平，停止行动
                if (_state != SoldierState::IDLE)
                {
                    stopAllActions();
                    _state = SoldierState::IDLE;
                }
                return;
            }
        }
    }

    // 3. 战斗执行逻辑
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

    // 4. 巡逻模式
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