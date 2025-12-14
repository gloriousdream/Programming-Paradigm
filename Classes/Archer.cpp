#include "Archer.h"

USING_NS_CC;

bool Archer::init()
{
    if (!Soldier::init()) return false;

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("archerwalk.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("archerattack.plist");

    // 远程攻击距离
    this->_attackRange = 250.0f;

    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("archer_side_walk_01.png");
    if (!frame) frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("archer_side_walk01.png");
    if (frame) this->setSpriteFrame(frame);

    this->setScale(0.8f);
    this->setAnchorPoint(Vec2(0.5, 0));

    return true;
}

void Archer::actionWalk()
{
    // 使用基类方向
    Vec2 diff = this->getCurrentDirection();
    if (diff.length() < 0.1f) diff = Vec2(1, 0);

    std::string animPrefix;

    if (diff.x < 0) this->setFlippedX(true);
    else this->setFlippedX(false);

    if (diff.y > std::abs(diff.x) * 0.5f)
    {
        animPrefix = "archer_upper_walk";
    }
    else if (diff.y < -std::abs(diff.x) * 0.5f)
    {
        animPrefix = "archer_under_walk";
    }
    else
    {
        animPrefix = "archer_side_walk";
    }

    this->stopActionByTag(999);

    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= 8; i++)
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

    if (!frames.empty())
    {
        auto anim = Animation::createWithSpriteFrames(frames, 0.1f);
        auto repeat = RepeatForever::create(Animate::create(anim));
        repeat->setTag(999);
        this->runAction(repeat);
    }
}

void Archer::actionAttack()
{
    if (!_targetBuilding) return;

    Vec2 diff = _targetBuilding->getPosition() - this->getPosition();

    if (diff.x < 0) this->setFlippedX(true);
    else this->setFlippedX(false);

    std::string animPrefix;
    if (diff.y > std::abs(diff.x) * 0.5f) animPrefix = "archer_upper_attack";
    else if (diff.y < -std::abs(diff.x) * 0.5f) animPrefix = "archer_under_attack";
    else animPrefix = "archer_side_attack";

    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= 9; i++)
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

    Animation* animation = Animation::createWithSpriteFrames(frames, 0.1f);
    Animate* animate = Animate::create(animation);

    auto doDamage = CallFunc::create([this]()
        {
            if (_targetBuilding && _targetBuilding->getParent())
            {
                _targetBuilding->takeDamage(15);
            }
        });

    auto seq = Sequence::create(animate, doDamage, nullptr);
    auto repeat = RepeatForever::create(seq);
    repeat->setTag(999);

    this->runAction(repeat);
}