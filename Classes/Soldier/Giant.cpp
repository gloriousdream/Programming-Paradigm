#include "Soldier/Giant.h"

USING_NS_CC;

bool Giant::init()
{
    if (!Soldier::init()) return false;

    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Soldiers/Giant/giantwalk.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Soldiers/Giant/giantattack.plist");

    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("giant_side_walk_01.png");
    if (!frame) frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("giant_side_walk01.png");
    if (frame) this->setSpriteFrame(frame);

    this->setScale(1.2f);
    this->setAnchorPoint(Vec2(0.5, 0));

    return true;
}

void Giant::actionWalk()
{
    Vec2 diff = this->getCurrentDirection();
    if (diff.length() < 0.1f) diff = Vec2(1, 0);

    std::string animPrefix;

    if (diff.x < 0) this->setFlippedX(true);
    else this->setFlippedX(false);

    if (diff.y > std::abs(diff.x) * 0.5f)
    {
        animPrefix = "giant_upper_walk";
    }
    else if (diff.y < -std::abs(diff.x) * 0.5f)
    {
        animPrefix = "giant_under_walk";
    }
    else
    {
        animPrefix = "giant_side_walk";
    }

    this->stopActionByTag(999);

    Vector<SpriteFrame*> frames;
    // 假设走路统一是 8 帧
    for (int i = 1; i <= 12; i++)
    {
        std::string name = StringUtils::format("%s_%02d.png", animPrefix.c_str(), i);
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        if (!frame)
        {
            name = StringUtils::format("%s%02d.png", animPrefix.c_str(), i);
            frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        }
        if (!frame) break;
        frames.pushBack(frame);
    }

    if (!frames.empty())
    {
        auto anim = Animation::createWithSpriteFrames(frames, 0.15f);
        auto repeat = RepeatForever::create(Animate::create(anim));
        repeat->setTag(999);
        this->runAction(repeat);
    }
}

void Giant::actionAttack()
{
    if (!_targetBuilding) return;

    Vec2 diff = _targetBuilding->getPosition() - this->getPosition();

    std::string animPrefix;
    int frameCount = 0;

    if (diff.x < 0) this->setFlippedX(true);
    else this->setFlippedX(false);

    if (diff.y > std::abs(diff.x) * 0.5f)
    {
        animPrefix = "giant_upper_attack";
        frameCount = 8;
    }
    else if (diff.y < -std::abs(diff.x) * 0.5f)
    {
        animPrefix = "giant_under_attack";
        frameCount = 8;
    }
    else
    {
        animPrefix = "giant_side_attack";
        frameCount = 20;
    }

    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= frameCount; i++)
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

    float frameRate = 0.1f;
    Animation* animation = Animation::createWithSpriteFrames(frames, frameRate);
    Animate* animate = Animate::create(animation);

    auto doDamage = CallFunc::create([this]()
        {
            if (_targetBuilding && _targetBuilding->getParent())
            {
                _targetBuilding->takeDamage(50);
            }
        });

    float delay = (frameCount == 8) ? 0.8f : 0.1f;

    auto seq = Sequence::create(animate, doDamage, DelayTime::create(delay), nullptr);
    auto repeat = RepeatForever::create(seq);
    repeat->setTag(999);

    this->runAction(repeat);
}