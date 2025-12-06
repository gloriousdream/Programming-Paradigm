#include "Building.h"
USING_NS_CC;

bool Building::init()
{
    if (!Sprite::init())
        return false;

    addTouchListener();
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