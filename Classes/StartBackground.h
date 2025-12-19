#ifndef __START_BACKGROUND_H__
#define __START_BACKGROUND_H__

#include "cocos2d.h"

class StartBackground : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();


    void menuStartGameCallback(cocos2d::Ref* pSender);
    // implement the "static create()" method manually
    CREATE_FUNC(StartBackground);
};

#endif // __START_BACKGROUND_H__