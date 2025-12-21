#include "Building.h"
USING_NS_CC;

bool Building::init()
{
    if (!Sprite::init()) return false;

    // 添加点击事件
    addTouchListener();

    // 创建血条 DrawNode 并调用 update
    hpBar = DrawNode::create();
    this->addChild(hpBar, 10);
    updateHPBar();

    return true;
}

void Building::addTouchListener()
{
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this](Touch* t, Event* e) {
        Vec2 localPos = this->convertToNodeSpace(t->getLocation());
        if (Rect(0, 0, getContentSize().width, getContentSize().height).containsPoint(localPos))
        {
            // 点击建筑 -> 发送 BUILDING_CLICKED
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("BUILDING_CLICKED", this);
            return true;
        }
        return false;
        };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void Building::updateUpgradeTimer(float dt)
{
    // 1. 扣除时间
    _remainingTime -= dt; // dt 通常是 1.0

    // 2. 检查是否结束
    if (_remainingTime <= 0)
    {
        // 时间到了！
        _remainingTime = 0;
        this->onUpgradeFinished(0); // 直接调用完成函数
    }
    else
    {
        // 3. 没结束，更新文字
        if (_timerLabel)
        {
            _timerLabel->setString(std::to_string((int)_remainingTime) + "s");
        }
    }
}
void Building::startUpgradeTimer(float duration)
{
    // 1. 标记状态
    this->setTag(0);

    // 2. 视觉预览 (变身 + 半透明)
    std::string nextImg = this->getNextLevelTextureName();
    if (!nextImg.empty())
    {
        this->setTexture(nextImg);
    }
    this->setOpacity(128);

    // 3. 初始化倒计时文字
    if (!_timerLabel)
    {
        // 如果还没有标签，就创建一个
        _timerLabel = Label::createWithTTF("", "fonts/Marker Felt.ttf", 24);
        _timerLabel->setColor(Color3B::WHITE);
        _timerLabel->enableOutline(Color4B::BLACK, 2); // 加个黑边，防止看不清
        // 放在建筑中心
        _timerLabel->setPosition(Vec2(getContentSize().width / 2, getContentSize().height / 2));
        this->addChild(_timerLabel, 100); // 层级设高点
    }

    // 4. 设置初始时间并显示
    _remainingTime = duration;
    _timerLabel->setString(std::to_string((int)_remainingTime) + "s");
    _timerLabel->setVisible(true);

    // 5. 改为每 1.0 秒执行一次 updateUpgradeTime
    this->schedule(CC_SCHEDULE_SELECTOR(Building::updateUpgradeTimer), 1.0f);

    CCLOG("Building started upgrading... Duration: %f", duration);
}
void Building::onUpgradeFinished(float dt)
{
    // 1. 真正的升级数据
    this->upgrade();

    // 2. 恢复视觉
    this->setOpacity(255);
    this->setTag(1);

    // 3. 停止倒计时定时器
    this->unschedule(CC_SCHEDULE_SELECTOR(Building::updateUpgradeTimer));

    // 4.隐藏倒计时文字
    if (_timerLabel)
    {
        _timerLabel->setVisible(false);
    }

    CCLOG("Upgrade Finished!");
}
// 立即完成 (也就是加速功能)
void Building::skipUpgradeTimer()
{
    // 直接调用完成函数
    onUpgradeFinished(0);
}
void Building::takeDamage(int dmg)
{
    // 如果已经死了，就不要再鞭尸了，直接返回
    if (_isDead) return;

    currentHP -= dmg;
    if (currentHP < 0) currentHP = 0;

    updateHPBar();

    if (currentHP <= 0)
    {
        _isDead = true; // 标记死亡

        this->setVisible(false);

        this->runAction(RemoveSelf::create());

        // 如果有血条，也隐藏掉
        if (hpBar) hpBar->setVisible(false);
    }
}

void Building::updateHPBar()
{
    if (!hpBar) return;
    hpBar->clear();

    // 血条相对建筑大小自适应
    float width = getContentSize().width * 0.8f; // 血条宽度占建筑宽度80%
    float height = 6;                           // 血条高度
    float x = (getContentSize().width - width) / 2;
    float y = getContentSize().height + 5;      // 血条显示在建筑上方5像素

    float hpPercent = (float)currentHP / (float)maxHP;
    if (hpPercent < 0) hpPercent = 0;
    if (hpPercent > 1) hpPercent = 1;

    Color4F backColor(0.3f, 0.3f, 0.3f, 1.0f);
    Color4F fillColor(0.0f, 1.0f, 0.0f, 1.0f);

    // 背景灰色矩形
    hpBar->drawSolidRect(Vec2(x, y), Vec2(x + width, y + height), backColor);
    // 当前血量绿色矩形
    hpBar->drawSolidRect(Vec2(x, y), Vec2(x + width * hpPercent, y + height), fillColor);
}
