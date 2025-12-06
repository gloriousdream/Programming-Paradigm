#include "Building.h"
USING_NS_CC;

bool Building::init()
{
    if (!Sprite::init())
        return false;

    addTouchListener();

    // 创建血条
    hpBar = DrawNode::create();
    this->addChild(hpBar, 10); // 显示在建筑顶部
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
            Director::getInstance()->getEventDispatcher()->dispatchCustomEvent("BUILDING_CLICKED", this);
            return true;
        }
        return false;
        };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
}

void Building::takeDamage(int dmg)
{
    currentHP -= dmg;
    if (currentHP < 0) currentHP = 0;
    updateHPBar();

    if (currentHP <= 0)
    {
        this->removeFromParentAndCleanup(true);
    }
}

void Building::updateHPBar()
{
    if (!hpBar) return;

    hpBar->clear();

    float width = getContentSize().width * 0.8f; // 血条宽度占建筑宽度80%
    float height = 6;                             // 血条高度
    float x = (getContentSize().width - width) / 2;
    float y = getContentSize().height + 5;       // 血条显示在建筑上方5像素

    float hpPercent = (float)currentHP / maxHP;
    Color4F backColor(0.3f, 0.3f, 0.3f, 1.0f);
    Color4F fillColor(0.0f, 1.0f, 0.0f, 1.0f);

    // 背景灰色
    hpBar->drawSolidRect(Vec2(x, y), Vec2(x + width, y + height), backColor);
    // 当前血量绿色
    hpBar->drawSolidRect(Vec2(x, y), Vec2(x + width * hpPercent, y + height), fillColor);
}
