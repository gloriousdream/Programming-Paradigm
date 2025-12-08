#include "Giant.h"

USING_NS_CC;

bool Giant::init()
{
    if (!Soldier::init()) return false;

    // 1. 加载巨人图集
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("giantwalk.plist");

    // 2. [关键] 设置初始静止状态为 03 帧
    // 假设默认显示侧面
    if (SpriteFrameCache::getInstance()->getSpriteFrameByName("giant_side_walk_03.png"))
    {
        this->setSpriteFrame("giant_side_walk_03.png");
    }
    else
    {
        this->setTexture("CloseNormal.png"); // 兜底
        CCLOGERROR("Giant resource not found!");
    }

    // 3. 巨人体型设置
    // 巨人通常比较大，如果不缩放或者放大一点会更有压迫感
    this->setScale(1.2f);
    this->setAnchorPoint(Vec2(0.5, 0)); // 脚底对齐

    return true;
}

Animate* Giant::createAnimate(const std::string& prefix, int frameCount)
{
    Vector<SpriteFrame*> frames;
    frames.reserve(frameCount);

    // [关键] 循环 1 到 12
    for (int i = 1; i <= frameCount; i++)
    {
        std::string name = StringUtils::format("%s_%02d.png", prefix.c_str(), i);
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        if (frame)
        {
            frames.pushBack(frame);
        }
    }

    // 巨人动作缓慢沉重，单帧时间设长一点 (0.15s)
    auto animation = Animation::createWithSpriteFrames(frames, 0.15f);
    return Animate::create(animation);
}

void Giant::actionWalk()
{
    // --- 1. 计算随机目标点 (巨人巡逻范围小一点，懒得动) ---
    float radius = 120.0f;
    float dx = (rand() % (int)(radius * 2)) - radius;
    float dy = (rand() % (int)(radius * 2)) - radius;
    Vec2 targetPos = homePosition + Vec2(dx, dy);

    Vec2 diff = targetPos - this->getPosition();

    // 距离太短就不动了
    if (diff.length() < 10.0f)
    {
        this->actionWalk();
        return;
    }

    // --- 2. 决定方向与翻转 ---
    std::string animPrefix = "";

    // 左右翻转 (素材全部朝右)
    if (diff.x < 0)
    {
        this->setFlippedX(true);  // 往左走，翻转
    }
    else
    {
        this->setFlippedX(false); // 往右走
    }

    // 上下侧面判断
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

    // --- 3. 运行动画 (1-12 循环) ---
    this->stopActionByTag(TAG_WALK_ACTION);

    Animate* anim = createAnimate(animPrefix, 12); // [关键] 12帧
    if (anim)
    {
        auto repeatAnim = RepeatForever::create(anim);
        repeatAnim->setTag(TAG_WALK_ACTION);
        this->runAction(repeatAnim);
    }

    // --- 4. 运行位移 ---
    // [关键] 巨人移动速度慢
    float speed = 35.0f;
    float duration = diff.length() / speed;
    if (duration < 0.1f) duration = 0.1f;

    auto move = MoveTo::create(duration, targetPos);

    auto finishMove = CallFunc::create([this, animPrefix]()
        {

            // A. 停止动画
            this->stopActionByTag(TAG_WALK_ACTION);

            // B. [关键] 强制恢复到 03 帧 (静止状态)
            std::string idleFrameName = StringUtils::format("%s_03.png", animPrefix.c_str());
            this->setSpriteFrame(idleFrameName);

            // C. 休息 (巨人反应慢，休息久一点)
            auto delay = DelayTime::create(2.0f + CCRANDOM_0_1() * 2.0f);
            auto next = CallFunc::create([this]()
                {
                    this->actionWalk();
                });

            this->runAction(Sequence::create(delay, next, nullptr));
        });

    this->runAction(Sequence::create(move, finishMove, nullptr));
}