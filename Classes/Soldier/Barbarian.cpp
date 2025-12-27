#include "Soldier/Barbarian.h"

USING_NS_CC;

bool Barbarian::init()
{
    if (!Soldier::init()) return false;

    // 加载资源
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("barbarianwalk.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("barbarianattack.plist");

    // 初始展示
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("barbarian_side_walk_01.png");
    if (!frame) frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("barbarian_side_walk01.png");
    if (frame) this->setSpriteFrame(frame);

    this->setScale(0.8f);
    this->setAnchorPoint(Vec2(0.5, 0));

    return true;
}

void Barbarian::actionWalk()
{
    // 使用基类计算好的瞬时方向
    Vec2 diff = this->getCurrentDirection();
    if (diff.length() < 0.1f) diff = Vec2(1, 0);

    std::string animPrefix;

    if (diff.x < 0) this->setFlippedX(true);
    else this->setFlippedX(false);

    if (diff.y > std::abs(diff.x) * 0.5f)
    {
        animPrefix = "barbarian_upper_walk";
    }
    else if (diff.y < -std::abs(diff.x) * 0.5f)
    {
        animPrefix = "barbarian_under_walk";
    }
    else
    {
        animPrefix = "barbarian_side_walk";
    }

    this->stopActionByTag(999);

    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= 8; i++)
    {
        // 兼容文件名格式
        std::string name = StringUtils::format("%s_%02d.png", animPrefix.c_str(), i);
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        if (!frame)
        {
            name = StringUtils::format("%s%02d.png", animPrefix.c_str(), i);
            frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        }
        if (frame) frames.pushBack(frame);
    }

    if (!frames.empty())
    {
        auto anim = Animation::createWithSpriteFrames(frames, 0.1f);
        auto repeat = RepeatForever::create(Animate::create(anim));
        repeat->setTag(999);
        this->runAction(repeat);
    }
}

void Barbarian::actionAttack()
{
    if (!_targetBuilding) return;

    // 攻击时依然面向目标
    Vec2 diff = _targetBuilding->getPosition() - this->getPosition();

    // 【可选】更新一下方向记录，防止打完转身太突兀
    _curMoveDir = diff.getNormalized();

    if (diff.x < 0) this->setFlippedX(true);
    else this->setFlippedX(false);

    std::string animPrefix;
    if (diff.y > std::abs(diff.x) * 0.5f) animPrefix = "barbarian_upper_attack";
    else if (diff.y < -std::abs(diff.x) * 0.5f) animPrefix = "barbarian_under_attack";
    else animPrefix = "barbarian_side_attack";

    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= 20; i++) // 假设攻击帧比较多
    {
        std::string name = StringUtils::format("%s_%02d.png", animPrefix.c_str(), i);
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        if (!frame)
        {
            name = StringUtils::format("%s%02d.png", animPrefix.c_str(), i);
            frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        }
        if (frame) frames.pushBack(frame);
    }

    if (frames.empty()) return;

    Animation* animation = Animation::createWithSpriteFrames(frames, 0.05f);
    Animate* animate = Animate::create(animation);

    auto doDamage = CallFunc::create([this]()
        {
            if (_targetBuilding && _targetBuilding->getParent())
            {
                _targetBuilding->takeDamage(20);
            }
        });

    auto seq = Sequence::create(animate, doDamage, DelayTime::create(0.1f), nullptr);
    auto repeat = RepeatForever::create(seq);
    repeat->setTag(999);
    this->runAction(repeat);
}