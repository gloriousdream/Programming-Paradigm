#include "Soldier.h"

USING_NS_CC;

bool Soldier::init()
{
    if (!Sprite::init()) return false;

    //  默认活动区域为全屏 (防止没设置区域时兵不动或崩溃)
    auto visibleSize = Director::getInstance()->getVisibleSize();
    _moveArea = Rect(0, 0, visibleSize.width, visibleSize.height);

    return true;
}

void Soldier::setHomePosition(Vec2 pos)
{
    this->homePosition = pos;
}

void Soldier::setMoveArea(const cocos2d::Rect& area)
{
    this->_moveArea = area;
}

// 在矩形内随机取一个坐标
Vec2 Soldier::getRandomPointInArea()
{
    // 在 [origin.x, origin.x + width] 之间随机
    float x = _moveArea.origin.x + CCRANDOM_0_1() * _moveArea.size.width;

    // 在 [origin.y, origin.y + height] 之间随机
    float y = _moveArea.origin.y + CCRANDOM_0_1() * _moveArea.size.height;

    return Vec2(x, y);
}
