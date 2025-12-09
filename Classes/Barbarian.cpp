#include "Barbarian.h"

USING_NS_CC;

bool Barbarian::init()
{
    if (!Soldier::init()) return false;

    // 1. 加载图集
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("barbarianwalk.plist");

    // 2. 设置初始外观为 07 (静止帧)
    // 默认先朝右看
    if (SpriteFrameCache::getInstance()->getSpriteFrameByName("barbarian_side_walk_07.png"))
    {
        this->setSpriteFrame("barbarian_side_walk_07.png");
    }
    else
    {
        // 防崩溃兜底
        this->setTexture("CloseNormal.png");
    }

    this->setScale(0.8f);
    this->setAnchorPoint(Vec2(0.5, 0));

    return true;
}
// 辅助：创建动画
Animate* Barbarian::createAnimate(const std::string& prefix, int frameCount)
{
    Vector<SpriteFrame*> frames;
    frames.reserve(frameCount);

    for (int i = 1; i <= frameCount; i++)
    {
        // 拼接名字
        std::string name = StringUtils::format("%s_%02d.png", prefix.c_str(), i);
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        if (frame)
        {
            frames.pushBack(frame);
        }
    }

    // 0.1f 是每一帧的时间，8帧就是0.8秒走一步
    auto animation = Animation::createWithSpriteFrames(frames, 0.1f);
    return Animate::create(animation);
}

void Barbarian::actionWalk()
{
    // 1. 计算随机目标点
    float radius = 150.0f;
    float dx = (rand() % (int)(radius * 2)) - radius;
    float dy = (rand() % (int)(radius * 2)) - radius;
    Vec2 targetPos = homePosition + Vec2(dx, dy);

    // 2. 计算方向向量
    Vec2 diff = targetPos - this->getPosition();

    // 如果距离太短（比如小于10像素），就不走了，重新随机
    if (diff.length() < 10.0f)
    {
        this->actionWalk();
        return;
    }

    // 3. 决定使用哪套动画 & 是否翻转
    std::string animPrefix = "";
    bool needFlipX = false;

    // 判断左右翻转 
    if (diff.x < 0)
    {
        needFlipX = true; // 目标在左边，翻转
    }
    else
    {
        needFlipX = false; // 目标在右边，不翻转
    }
    this->setFlippedX(needFlipX);

    // 判断是用 Side, Upper 还是 Under
    // 用斜率来判断：如果纵向移动比横向明显，就用上下走，否则用侧着走
    // 这里设定一个阈值，比如 tan(30度) 左右，或者简单判断 dy 和 dx 的绝对值

    if (diff.y > std::abs(diff.x) * 0.5f)
    {
        // 往上走 (Y 正方向，且分量够大)
        animPrefix = "barbarian_upper_walk";
    }
    else if (diff.y < -std::abs(diff.x) * 0.5f)
    {
        // 往下走 (Y 负方向)
        animPrefix = "barbarian_under_walk";
    }
    else
    {
        // 主要是横向移动
        animPrefix = "barbarian_side_walk";
    }

    // 运行动画 (1-8 循环) 
    this->stopActionByTag(TAG_WALK_ACTION);
    Animate* anim = createAnimate(animPrefix, 8);
    if (anim)
    {
        auto repeatAnim = RepeatForever::create(anim);
        repeatAnim->setTag(TAG_WALK_ACTION);
        this->runAction(repeatAnim);
    }

    // 运行位移 
    float speed = 60.0f;
    float duration = diff.length() / speed;

    // 限制一下最小移动时间，防止瞬间移动造成的闪烁
    if (duration < 0.1f) duration = 0.1f;

    auto move = MoveTo::create(duration, targetPos);

    // 移动结束后的回调
    auto finishMove = CallFunc::create([this, animPrefix]()
        {

            // 1. 立即停止走路动画
            this->stopActionByTag(TAG_WALK_ACTION);

            // 2. 强制恢复到 07 帧 (静止状态)
            // 这样不管他刚刚是侧走、上走还是下走，都会停在对应的 "07" 姿势上
            std::string idleFrameName = StringUtils::format("%s_07.png", animPrefix.c_str());
            this->setSpriteFrame(idleFrameName);

            // 3. 休息逻辑 (休息时保持 07 的样子)
            auto delay = DelayTime::create(1.0f + CCRANDOM_0_1() * 2.0f);
            auto next = CallFunc::create([this]()
                {
                    this->actionWalk(); // 再次出发
                });

            this->runAction(Sequence::create(delay, next, nullptr));
        });

    this->runAction(Sequence::create(move, finishMove, nullptr));
}