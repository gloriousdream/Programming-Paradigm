#include "CoinCollection.h"

USING_NS_CC;

bool CoinCollection::init()
{
    // 1. 初始化基类
    if (!Building::init()) return false;

    // 2. 设置初始贴图和属性 (保留你原有的数值)
    setTexture("CoinCollection.png");
    level = 1;
    maxHP = 150;
    currentHP = maxHP;
    updateHPBar();

    // 3. 设置建造消耗
    buildCostGold = 20;
    buildCostHoly = 80;

    // 4. 设置升级消耗
    upgradeCostGold = 40;
    upgradeCostHoly = 100;

    // ---------------------------------------------------------
    // 【新增】金币生产逻辑
    // ---------------------------------------------------------

    // 5. 创建悬浮的 Coin.png 图标 (请确保 Resources 目录下有 Coin.png)
    rewardIcon = Sprite::create("Coin.png");
    if (rewardIcon)
    {
        // 放在建筑头顶上方
        rewardIcon->setPosition(getContentSize().width / 2, getContentSize().height + 40);
        rewardIcon->setVisible(false); // 初始隐藏

        // 上下浮动动画
        auto moveUp = MoveBy::create(1.0f, Vec2(0, 10));
        auto moveDown = moveUp->reverse();
        rewardIcon->runAction(RepeatForever::create(Sequence::create(moveUp, moveDown, nullptr)));

        this->addChild(rewardIcon, 20); // 层级在建筑之上
    }

    // 6. 第一次启动：等待 10秒 产出
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(CoinCollection::produceResource), 10.0f);

    // 7. 添加点击监听器 (专门用来检测图标点击)
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
            return true; // 吞噬点击
        }

        return false;
        };
    // 优先级设为比 Building 基类的监听器更高
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    return true;
}

// 定时器回调：显示图标
void CoinCollection::produceResource(float dt)
{
    if (rewardIcon)
    {
        rewardIcon->setVisible(true);
        isReadyToCollect = true;
    }
}

// 玩家点击收集
void CoinCollection::onCollect()
{
    if (!isReadyToCollect) return;

    CCLOG("Collected 5 Gold!");

    // 1. 隐藏图标
    if (rewardIcon) {
        rewardIcon->setVisible(false);
    }
    isReadyToCollect = false;

    // 2. 发送自定义事件给 GameScene 加金币
    // 事件名定义为 COLLECT_COIN_EVENT
    int amount = 5;
    EventCustom event("COLLECT_COIN_EVENT");
    event.setUserData(&amount);
    _eventDispatcher->dispatchEvent(&event);

    // 3. 收集后，等待 10 秒再次生成
    this->scheduleOnce(CC_SCHEDULE_SELECTOR(CoinCollection::produceResource), 10.0f);
}

void CoinCollection::upgrade()
{
    level++;
    if (level == 2) {
        setTexture("CoinCollection2.png");
        maxHP = 200;
        upgradeCostGold = 80;
        upgradeCostHoly = 150;
    }
    else if (level == 3) {
        setTexture("CoinCollection3.png");
        maxHP = 300;
    }

    currentHP = maxHP;
    updateHPBar();
}