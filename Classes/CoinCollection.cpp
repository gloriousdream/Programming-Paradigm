#include "CoinCollection.h"

USING_NS_CC;
std::string CoinCollection::getNextLevelTextureName()
{
    int nextLv = level + 1;

    if (nextLv == 2) return "CoinCollection.png";
    if (nextLv == 3) return "CoinCollection.png";

    return "";
}
bool CoinCollection::init()
{
    if (!Building::init()) return false;

    setTexture("CoinCollection.png");
    level = 1;
    maxHP = 150;
    currentHP = maxHP;
    updateHPBar();

    buildCostGold = 20;
    buildCostHoly = 80;
    upgradeCostGold = 40;
    upgradeCostHoly = 100;

    // 初始化
    currentStorage = 0;
    maxStorage = 100;

    rewardIcon = Sprite::create("Coin.png");
    if (rewardIcon)
    {
        rewardIcon->setPosition(getContentSize().width / 2, getContentSize().height + 40);
        rewardIcon->setVisible(false);

        auto moveUp = MoveBy::create(1.0f, Vec2(0, 10));
        auto moveDown = moveUp->reverse();
        rewardIcon->runAction(RepeatForever::create(Sequence::create(moveUp, moveDown, nullptr)));

        this->addChild(rewardIcon, 20);

        // 绑定到 rewardIcon
        auto touchListener = EventListenerTouchOneByOne::create();
        touchListener->setSwallowTouches(true);

        touchListener->onTouchBegan = [=](Touch* t, Event* e) {
            auto target = static_cast<Sprite*>(e->getCurrentTarget());
            if (!target->isVisible()) return false;

            Vec2 pos = target->convertToNodeSpace(t->getLocation());
            Size s = target->getContentSize();
            Rect rect = Rect(0, 0, s.width, s.height);

            if (rect.containsPoint(pos))
            {
                this->onCollect();
                return true;
            }
            return false;
            };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, rewardIcon);
    }

    // 每 5 秒产出一次
    this->schedule(CC_SCHEDULE_SELECTOR(CoinCollection::produceResource), 5.0f);

    return true;
}

void CoinCollection::produceResource(float dt)
{
    if (currentStorage >= maxStorage) return;

    int production = level * 5;
    currentStorage += production;

    if (currentStorage > maxStorage) currentStorage = maxStorage;

    if (currentStorage > 0 && rewardIcon) {
        rewardIcon->setVisible(true);
    }
}

void CoinCollection::onCollect()
{
    if (currentStorage <= 0) return;

    int amount = currentStorage;

    CCLOG("Collected %d Gold!", amount);

    EventCustom event("COLLECT_COIN_EVENT");
    event.setUserData(&amount);
    _eventDispatcher->dispatchEvent(&event);

    currentStorage = 0;
    if (rewardIcon) {
        rewardIcon->setVisible(false);
    }
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

    // 升级增加容量
    maxStorage = level * 100;

    currentHP = maxHP;
    updateHPBar();
}

void CoinCollection::setEnemyState(bool isEnemy)
{
    if (isEnemy)
    {
        // 停止生产
        this->unschedule(CC_SCHEDULE_SELECTOR(CoinCollection::produceResource));

        // 移除图标
        if (rewardIcon) {
            rewardIcon->removeFromParent();
            rewardIcon = nullptr;
        }
        currentStorage = 0;
    }
}