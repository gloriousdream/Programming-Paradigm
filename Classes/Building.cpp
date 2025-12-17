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
