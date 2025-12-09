#include "Archer.h"

USING_NS_CC;

bool Archer::init()
{
    if (!Soldier::init()) return false;

    // 1. 加载弓箭手图集
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("archerwalk.plist");

    // 2. 设置初始静止状态 (07帧)
    // 假设默认显示侧面
    if (SpriteFrameCache::getInstance()->getSpriteFrameByName("archer_side_walk_07.png"))
    {
        this->setSpriteFrame("archer_side_walk_07.png");
    }
    else
    {
        // 兜底：如果资源没加载成功，用个色块或默认图防止崩溃
        this->setTexture("CloseNormal.png");
        CCLOGERROR("Archer resource not found!");
    }

    // 弓箭手通常比野蛮人稍微瘦一点，保持 0.8 或根据实际美术调整
    this->setScale(0.8f);
    this->setAnchorPoint(Vec2(0.5, 0)); // 脚底对齐

    return true;
}

// 辅助：创建动画 (1-8帧)
Animate* Archer::createAnimate(const std::string& prefix, int frameCount)
{
    Vector<SpriteFrame*> frames;
    frames.reserve(frameCount);

    for (int i = 1; i <= frameCount; i++)
    {
        // 格式化文件名：archer_side_walk_01.png
        std::string name = StringUtils::format("%s_%02d.png", prefix.c_str(), i);
        auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(name);
        if (frame)
        {
            frames.pushBack(frame);
        }
    }

    // 弓箭手可能动作轻快一点，0.1f 是标准速度
    auto animation = Animation::createWithSpriteFrames(frames, 0.1f);
    return Animate::create(animation);
}

void Archer::actionWalk()
{
    // 1. 计算随机目标点 (巡逻半径可以比野蛮人稍大或稍小) 
    float radius = 180.0f; // 弓箭手巡逻范围稍大
    float dx = (rand() % (int)(radius * 2)) - radius;
    float dy = (rand() % (int)(radius * 2)) - radius;
    Vec2 targetPos = homePosition + Vec2(dx, dy);

    // 2. 计算方向向量
    Vec2 diff = targetPos - this->getPosition();

    // 距离过短则重试
    if (diff.length() < 10.0f)
    {
        this->actionWalk();
        return;
    }

    // 3. 决定动画前缀 & 翻转
    std::string animPrefix = "";

    // 左右翻转 (素材全部朝右)
    if (diff.x < 0)
    {
        this->setFlippedX(true);  // 往左走，翻转
    }
    else
    {
        this->setFlippedX(false); // 往右走，正常
    }

    // 上下侧面判断 (逻辑同野蛮人)
    // 如果 Y 轴分量显著大于 X 轴的一半，则视为纵向移动
    if (diff.y > std::abs(diff.x) * 0.5f)
    {
        animPrefix = "archer_upper_walk"; // 背影
    }
    else if (diff.y < -std::abs(diff.x) * 0.5f)
    {
        animPrefix = "archer_under_walk"; // 正面
    }
    else
    {
        animPrefix = "archer_side_walk";  // 侧面
    }

    // 4. 运行动画
    this->stopActionByTag(TAG_WALK_ACTION);

    // 播放 01-08 的循环动画
    Animate* anim = createAnimate(animPrefix, 8);
    if (anim)
    {
        auto repeatAnim = RepeatForever::create(anim);
        repeatAnim->setTag(TAG_WALK_ACTION);
        this->runAction(repeatAnim);
    }

    // 5. 运行位移
    float speed = 70.0f; // 弓箭手移动速度稍快
    float duration = diff.length() / speed;
    if (duration < 0.1f) duration = 0.1f;

    auto move = MoveTo::create(duration, targetPos);

    // 移动结束回调
    auto finishMove = CallFunc::create([this, animPrefix]()
        {

            // 1. 停止动画
            this->stopActionByTag(TAG_WALK_ACTION);

            // 2. 恢复到 07 帧 (静止状态)
            // 根据刚才行走的方向，选择对应的静止图
            std::string idleFrameName = StringUtils::format("%s_07.png", animPrefix.c_str());
            this->setSpriteFrame(idleFrameName);

            // 3. 休息后继续
            auto delay = DelayTime::create(1.0f + CCRANDOM_0_1() * 2.0f);
            auto next = CallFunc::create([this]()
                {
                    this->actionWalk();
                });

            this->runAction(Sequence::create(delay, next, nullptr));
        });

    this->runAction(Sequence::create(move, finishMove, nullptr));
}