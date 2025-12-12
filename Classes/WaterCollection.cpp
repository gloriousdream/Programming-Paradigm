#include "WaterCollection.h"

USING_NS_CC;

bool WaterCollection::init()
{
    // 1. 初始化基类
    if (!Building::init()) return false;

    // 2. 基础属性
    setTexture("WaterCollection.png");
    level = 1;
    maxHP = 120;
    currentHP = maxHP;
    updateHPBar();

    buildCostGold = 80;
    buildCostHoly = 80;
    upgradeCostGold = 40;
    upgradeCostHoly = 40;

    // 初始化库存
    currentStorage = 0;
    maxStorage = 100; // 1级上限

    // 3. 创建悬浮图标
    rewardIcon = Sprite::create("Water.png");
    if (rewardIcon)
    {
        rewardIcon->setPosition(getContentSize().width / 2, getContentSize().height + 40);
        rewardIcon->setVisible(false); // 初始隐藏

        // 浮动动画
        auto moveUp = MoveBy::create(1.0f, Vec2(0, 10));
        auto moveDown = moveUp->reverse();
        rewardIcon->runAction(RepeatForever::create(Sequence::create(moveUp, moveDown, nullptr)));

        this->addChild(rewardIcon, 20);

        // 触摸监听器直接绑定到 rewardIcon 上
        auto touchListener = EventListenerTouchOneByOne::create();
        touchListener->setSwallowTouches(true); // 吞噬事件，防止点穿到建筑

        touchListener->onTouchBegan = [=](Touch* t, Event* e) {
            // 获取事件目标 (就是 rewardIcon)
            auto target = static_cast<Sprite*>(e->getCurrentTarget());

            // 如果图标没显示，肯定不能点
            if (!target->isVisible()) return false;

            // 转换坐标到图标内部
            Vec2 pos = target->convertToNodeSpace(t->getLocation());
            Size s = target->getContentSize();
            Rect rect = Rect(0, 0, s.width, s.height);

            // 判断是否点中图标
            if (rect.containsPoint(pos))
            {
                this->onCollect(); // 触发收集
                return true;       // 吞噬本次点击
            }
            return false;
            };

        // 绑定到 rewardIcon
        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, rewardIcon);
    }

    // 4. 开启无限循环生产 (每 5 秒生产一次)
    this->schedule(CC_SCHEDULE_SELECTOR(WaterCollection::produceResource), 5.0f);

    return true;
}

// 生产逻辑：累加资源
void WaterCollection::produceResource(float dt)
{
    // 如果满了就不产了
    if (currentStorage >= maxStorage) return;

    // 产量公式：等级 * 5
    int production = level * 5;

    currentStorage += production;

    // 超过上限截断
    if (currentStorage > maxStorage) {
        currentStorage = maxStorage;
    }

    // 只要有库存，就显示图标
    if (currentStorage > 0 && rewardIcon) {
        rewardIcon->setVisible(true);
    }
}

// 收集逻辑：一次性拿走所有
void WaterCollection::onCollect()
{
    if (currentStorage <= 0) return;

    int amount = currentStorage;

    CCLOG("Collected %d HolyWater!", amount);

    // 1. 发送事件给 GameScene 加资源
    EventCustom event("COLLECT_WATER_EVENT");
    event.setUserData(&amount);
    _eventDispatcher->dispatchEvent(&event);

    // 2. 清空库存
    currentStorage = 0;

    // 3. 隐藏图标
    if (rewardIcon) {
        rewardIcon->setVisible(false);
    }
}

void WaterCollection::upgrade()
{
    level++;
    // 升级逻辑示例
    if (level == 2) {
        setTexture("WaterCollection.png"); // 如果有新图换新图
        maxHP = 150;
    }
    else if (level == 3) {
        setTexture("WaterCollection.png");
        maxHP = 180;
    }

    // 升级增加容量上限
    maxStorage = level * 100;

    currentHP = maxHP;
    updateHPBar();
}

void WaterCollection::setEnemyState(bool isEnemy)
{
    if (isEnemy)
    {
        // 1. 停止生产定时器
        this->unschedule(CC_SCHEDULE_SELECTOR(WaterCollection::produceResource));

        // 2. 隐藏图标，防止由于初始化时的动画导致它显示出来
        if (rewardIcon) {
            rewardIcon->setVisible(false);
            rewardIcon->stopAllActions(); // 停止浮动动画，节省性能
            rewardIcon->removeFromParent(); // 直接移除图标更彻底
            rewardIcon = nullptr;
        }

        // 3. 清空库存
        currentStorage = 0;
    }
}