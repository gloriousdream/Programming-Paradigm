#include "Soldier.h"

USING_NS_CC;

bool Soldier::init()
{
    if (!Sprite::init()) return false;
    return true;
}

void Soldier::setHomePosition(Vec2 pos)
{
    this->homePosition = pos;
}

void Soldier::executeRandomWalk(float radius)
{
    // 1. 在半径范围内随机找一个目标点
    float dx = (rand() % (int)(radius * 2)) - radius; // -radius 到 +radius
    float dy = (rand() % (int)(radius * 2)) - radius;

    Vec2 targetPos = homePosition + Vec2(dx, dy);

    // 2. 简单的朝向翻转 (如果往左走就翻转)
    if (targetPos.x < this->getPositionX())
    {
        this->setFlippedX(false); // 假设原图朝左，或者根据原图调整
    }
    else
    {
        this->setFlippedX(true);
    }

    // 3. 计算距离和时间 (假设速度恒定)
    float distance = this->getPosition().distance(targetPos);
    float speed = 50.0f; // 像素/秒
    float duration = distance / speed;

    // 4. 创建动作序列：移动 -> 停顿 -> 再走
    auto move = MoveTo::create(duration, targetPos);
    auto delay = DelayTime::create(1.0f + CCRANDOM_0_1() * 2.0f); // 停1~3秒
    auto nextWalk = CallFunc::create([this]()
        {
            this->actionWalk(); // 递归调用子类的接口
        });

    this->runAction(Sequence::create(move, delay, nextWalk, nullptr));
}