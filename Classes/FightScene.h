#ifndef __FIGHT_SCENE_H__
#define __FIGHT_SCENE_H__

#include "cocos2d.h"

class FightScene : public cocos2d::Scene
{
public:
    // 修改 createScene，接收难度参数
    // difficulty: 1=Easy, 2=Middle, 3=Hard
    static cocos2d::Scene* createScene(int difficulty);

    virtual bool init() override;

    // 自定义初始化函数，用来处理难度
    bool initWithDifficulty(int difficulty);

    // 手动实现 create 宏的部分逻辑，因为我们需要传参
    static FightScene* create(int difficulty);

private:
    int _difficulty; // 存储当前场景的难度
};

#endif // __FIGHT_SCENE_H__