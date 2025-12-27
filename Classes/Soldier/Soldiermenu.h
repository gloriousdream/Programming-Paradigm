#ifndef __SOLDIER_MENU_H__
#define __SOLDIER_MENU_H__
#include "cocos2d.h"

class Soldiermenu : public cocos2d::Layer
{
public:
    static Soldiermenu* createMenu();

    // 修改回调：返回 (士兵类型, 训练数量)
    std::function<void(int type, int amount)> onTrainSoldier;
    virtual bool init();
    CREATE_FUNC(Soldiermenu);

private:
    // 辅助函数：构建单个士兵的控制单元
    void createSoldierUnit(const std::string& imgName, int type, cocos2d::Vec2 pos);
};
#endif

