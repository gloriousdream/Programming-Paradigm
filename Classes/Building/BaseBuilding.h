#ifndef __BASE_BUILDING_H__
#define __BASE_BUILDING_H__

#include "cocos2d.h"

class BaseBuilding : public cocos2d::Sprite
{
public:
    virtual bool initWithType(int type);

    virtual void upgrade() = 0;        // 子类必须实现
    virtual void updateTexture() = 0;  // 子类必须更新贴图

    int getLevel() const { return _level; }
    int getType()  const { return _type; }

protected:
    int _level = 1;   // 默认一级
    int _type = 0;   // 1 军营 / 2 水井 / 3 箭塔
};

#endif
