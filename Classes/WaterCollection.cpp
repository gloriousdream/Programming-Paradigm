#include "WaterCollection.h"

USING_NS_CC;

bool WaterCollection::init()
{
    if (!Building::init()) return false;

    // 1. 基础属性
    setTexture("WaterCollection.png");
    level = 1;
    maxHP = 120;
    currentHP = maxHP;
    updateHPBar();

    buildCostGold = 80;
    buildCostHoly = 80;
    upgradeCostGold = 40;
    upgradeCostHoly = 40;

    // 2. 创建悬浮的 Water.png 图标 
    rewardIcon = Sprite::create("Water.png");
    if (rewardIcon)
    {
        // 放在建筑头顶上方
        rewardIcon->setPosition(getContentSize().width / 2, getContentSize().height + 40);
        rewardIcon->setVisible(false); // 初始隐藏

        // 加上浮动动画
        auto moveUp = MoveBy::create(1.0f, Vec2(0, 10));
        auto moveDown = moveUp->reverse();
        rewardIcon->runAction(RepeatForever::create(Sequence::create(moveUp, moveDown, nullptr)));

        this->addChild(rewardIcon, 20); // 层级在建筑之上
    }

    // 3. 第一次启动：等待 10秒 产出
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(WaterCollection::produceResource), 10.0f);

    // 4. 添加点击监听器 (专门用来检测图标点击)
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true); // 吞噬事件

    touchListener->onTouchBegan = [this](Touch* t, Event* e) {
        // 如果不可收集或图标没显示，不处理
        if (!isReadyToCollect || !rewardIcon || !rewardIcon->isVisible()) return false;

        // 转换坐标检测点击
        Vec2 touchPos = rewardIcon->convertTouchToNodeSpace(t);
        Size s = rewardIcon->getContentSize();
        Rect rect(0, 0, s.width, s.height);

        // 如果点到了图标
        if (rect.containsPoint(touchPos))
        {
            this->onCollect();
            return true; // 吞噬点击，防止触发建筑升级菜单
        }

        return false;
        };
    // 优先级设为比 Building 基类的监听器更高
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    return true;
}

// 定时器回调：显示图标
void WaterCollection::produceResource(float dt)
{
    if (rewardIcon)
    {
        rewardIcon->setVisible(true);
        isReadyToCollect = true;
    }
}

// 玩家点击收集
void WaterCollection::onCollect()
{
    if (!isReadyToCollect) return;

    CCLOG("Collected 5 HolyWater!");

    // 1. 隐藏图标
    if (rewardIcon) {
        rewardIcon->setVisible(false);
    }
    isReadyToCollect = false;

    // 2. 发送自定义事件给 GameScene 加圣水
    // 【修改点】名字改为 COLLECT_WATER_EVENT
    int amount = 5;
    EventCustom event("COLLECT_WATER_EVENT");
    event.setUserData(&amount);
    _eventDispatcher->dispatchEvent(&event);

    // 3. 收集后，等待 10 秒再次生成
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(WaterCollection::produceResource), 10.0f);
}

void WaterCollection::upgrade()
{
    level++;
    if (level == 2) {
        setTexture("WaterCollection.png");
        maxHP = 150;
    }
    else if (level == 3) {
        setTexture("WaterCollection.png");
        maxHP = 180;
    }
    currentHP = maxHP;
    updateHPBar();
}