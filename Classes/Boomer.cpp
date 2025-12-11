#include "Boomer.h"

USING_NS_CC;

bool Bomber::init()
{
    if (!Soldier::init()) return false;

    // 1. 加载炸弹人图集
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("wallbreakerwalk.plist");

    // 2. [关键] 设置初始静止状态 (06帧)
    // 默认显示侧面
    std::string startFrame = "wall_breaker_side_walk_06.png";
    if (SpriteFrameCache::getInstance()->getSpriteFrameByName(startFrame))
    {
        this->setSpriteFrame(startFrame);
    }
    else
    {
        // 兜底防止崩溃
        this->setTexture("CloseNormal.png");
        CCLOGERROR("Bomber resource not found: %s", startFrame.c_str());
    }

    // 炸弹人个头通常比较小，但拿着大炸弹
    this->setScale(0.8f);
    this->setAnchorPoint(Vec2(0.5, 0)); // 脚底对齐

    return true;
}

// 辅助：创建动画 (1-8帧)
Animate* Bomber::createAnimate(const std::string& prefix, int frameCount)
{
    Vector<SpriteFrame*> frames;
    frames.reserve(frameCount);

    for (int i = 1; i <= frameCount; i++)
    {
        // 格式化文件名：wall_breaker_side_walk_01.png
        std::string name = StringUtils::format("%s_%02d.png", prefix.c_str(), i);
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        if (frame)
        {
            frames.pushBack(frame);
        }
    }

    // 炸弹人步伐急促，速度设快一点 (0.08s)
    auto animation = Animation::createWithSpriteFrames(frames, 0.08f);
    return Animate::create(animation);
}

void Bomber::actionWalk()
{
    Vec2 targetPos = this->getRandomPointInArea();
    // --- 2. 计算方向向量 ---
    Vec2 diff = targetPos - this->getPosition();

    // 距离过短则重试
    if (diff.length() < 10.0f)
    {
        this->actionWalk();
        return;
    }

    // --- 3. 决定动画前缀 & 翻转 ---
    std::string animPrefix = "";

    // A. 左右翻转 (素材全部朝右)
    if (diff.x < 0)
    {
        this->setFlippedX(true);  // 往左走，翻转
    }
    else
    {
        this->setFlippedX(false); // 往右走，正常
    }

    // B. 上下侧面判断
    // 阈值判断：Y轴移动量 > X轴移动量的一半
    if (diff.y > std::abs(diff.x) * 0.5f)
    {
        animPrefix = "wall_breaker_upper_walk"; // 背影
    }
    else if (diff.y < -std::abs(diff.x) * 0.5f)
    {
        animPrefix = "wall_breaker_under_walk"; // 正面
    }
    else
    {
        animPrefix = "wall_breaker_side_walk";  // 侧面
    }

    // --- 4. 运行动画 ---
    this->stopActionByTag(TAG_WALK_ACTION);

    // 播放 01-08 的循环动画
    Animate* anim = createAnimate(animPrefix, 8);
    if (anim)
    {
        auto repeatAnim = RepeatForever::create(anim);
        repeatAnim->setTag(TAG_WALK_ACTION);
        this->runAction(repeatAnim);
    }

    // --- 5. 运行位移 ---
    // 炸弹人移动速度很快
    float speed = 90.0f;
    float duration = diff.length() / speed;
    if (duration < 0.1f) duration = 0.1f;

    auto move = MoveTo::create(duration, targetPos);

    // 移动结束回调
    auto finishMove = CallFunc::create([this, animPrefix]()
        {

            // 1. 停止动画
            this->stopActionByTag(TAG_WALK_ACTION);

            // 2. [关键] 恢复到 06 帧 (静止状态)
            // 无论刚才朝向哪里，停下来都显示对应的 06 帧
            std::string idleFrameName = StringUtils::format("%s_06.png", animPrefix.c_str());
            this->setSpriteFrame(idleFrameName);

            // 3. 休息后继续
            auto delay = DelayTime::create(0.5f + CCRANDOM_0_1() * 1.5f);
            auto next = CallFunc::create([this]()
                {
                    this->actionWalk();
                });

            this->runAction(Sequence::create(delay, next, nullptr));
        });

    this->runAction(Sequence::create(move, finishMove, nullptr));
}