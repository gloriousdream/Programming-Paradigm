#include "Soldier/Boomer.h"

USING_NS_CC;

bool Bomber::init()
{
    if (!Soldier::init()) return false;

    // 加载动画资源
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Soldiers/Bomber/wallbreakerwalk.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("Soldiers/Bomber/wallbreakerattack.plist");

    // 初始展示
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("wall_breaker_side_walk_01.png");
    if (!frame) frame = SpriteFrameCache::getInstance()->getSpriteFrameByName("wall_breaker_side_walk01.png");
    if (frame) this->setSpriteFrame(frame);

    this->setScale(0.8f);
    this->setAnchorPoint(Vec2(0.5, 0));

    // 炸弹人攻击距离很近，几乎贴脸
    this->_attackRange = 40.0f;

    return true;
}

void Bomber::actionWalk()
{
    // 获取移动方向
    Vec2 diff = this->getCurrentDirection();
    if (diff.length() < 0.1f) diff = Vec2(1, 0);

    std::string animPrefix;

    if (diff.x < 0) this->setFlippedX(true);
    else this->setFlippedX(false);

    if (diff.y > std::abs(diff.x) * 0.5f)
    {
        animPrefix = "wall_breaker_upper_walk";
    }
    else if (diff.y < -std::abs(diff.x) * 0.5f)
    {
        animPrefix = "wall_breaker_under_walk";
    }
    else
    {
        animPrefix = "wall_breaker_side_walk";
    }

    // 防止重复设置相同的动作
    this->stopActionByTag(999);

    Vector<SpriteFrame*> frames;
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
        auto anim = Animation::createWithSpriteFrames(frames, 0.08f);
        auto repeat = RepeatForever::create(Animate::create(anim));
        repeat->setTag(999);
        this->runAction(repeat);
    }
}

void Bomber::actionAttack()
{
    // 如果已经在进行攻击（正在扔炸弹），不要重复触发
    // 我们可以通过 Tag 或者 State 来判断，这里直接 stopAllActions 简单粗暴
    this->stopAllActions();

    if (!_targetBuilding) return;

    Vec2 diff = _targetBuilding->getPosition() - this->getPosition();

    // 1. 设置朝向
    if (diff.x < 0) this->setFlippedX(true);
    else this->setFlippedX(false);

    std::string animPrefix;
    if (diff.y > std::abs(diff.x) * 0.5f) animPrefix = "wall_breaker_upper_attack";
    else if (diff.y < -std::abs(diff.x) * 0.5f) animPrefix = "wall_breaker_under_attack";
    else animPrefix = "wall_breaker_side_attack";

    // 2. 准备动画帧 (1-7帧)
    Vector<SpriteFrame*> frames;
    for (int i = 1; i <= 7; i++)
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

    // 3. 创建动画动作 (只播一次，不要 RepeatForever)
    Animation* animation = Animation::createWithSpriteFrames(frames, 0.1f);
    Animate* animate = Animate::create(animation);

    // 4. 【核心逻辑】扔炸弹 -> 爆炸 -> 自杀
    auto throwBombSequence = CallFunc::create([this, animPrefix]()
        {

            // A. 创建炸弹精灵
            auto bomb = Sprite::create("Soldiers/Bomber/wall_breaker_bomb.png");
            if (bomb)
            {
                // 炸弹位置：放在炸弹人脚下或稍前面
                bomb->setPosition(this->getPosition() + Vec2(0, 20));

                // 将炸弹添加到 Soldier 的父节点(FightScene)，确保炸弹人死后炸弹还在
                if (this->getParent())
                {
                    this->getParent()->addChild(bomb, this->getLocalZOrder() + 1);
                }

                // B. 炸弹人动作：回到第一帧 (假装扔完看着)
                std::string idleFrameName = StringUtils::format("%s_01.png", animPrefix.c_str());
                auto idleFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(idleFrameName);
                if (!idleFrame) idleFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(StringUtils::format("%s01.png", animPrefix.c_str()));
                if (idleFrame)
                {
                    this->setSpriteFrame(idleFrame);
                }

                // C. 炸弹的生命周期：延迟0.5s -> 爆炸造成伤害 -> 炸弹消失 -> 炸弹人死亡
                auto bombLogic = Sequence::create(
                    DelayTime::create(0.5f),
                    CallFunc::create([this, bomb]()
                        {
                            // 1. 造成伤害
                            if (this->_targetBuilding && this->_targetBuilding->getParent())
                            {
                                // 炸弹人伤害极高，假设 200
                                this->_targetBuilding->takeDamage(200);

                            }

                            // 2. 移除炸弹
                            bomb->removeFromParent();

                            // 3. 移除炸弹人 (自杀)
                            this->removeFromParent();
                        }),
                    nullptr
                );

                bomb->runAction(bombLogic);
            }
        });

    // 5. 运行序列： 播放攻击动画 -> 执行扔炸弹逻辑
    auto finalSeq = Sequence::create(animate, throwBombSequence, nullptr);
    finalSeq->setTag(999);
    this->runAction(finalSeq);
}