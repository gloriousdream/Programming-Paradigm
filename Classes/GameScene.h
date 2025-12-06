#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);

    void onBuildingClicked(cocos2d::Sprite* building);
    void showUpgradeButton(cocos2d::Sprite* building);
    void updateResourceDisplay();

private:
    bool placeModebuild = false;
    bool placeModesoldier = false;
    int selectedType = 0;

    // 玩家资源
    int gold = 200;
    int holyWater = 100;

    cocos2d::Label* goldLabel = nullptr;
    cocos2d::Label* waterLabel = nullptr;

    void onSoldierpushed();
    void onBuildButtonPressed();
    template<typename T>
    void enablePlaceMode(int type, T menu);
    void onMapClicked(cocos2d::Vec2 pos);

    // 当前显示升级菜单的建筑
    cocos2d::Sprite* currentBuildingMenu = nullptr;
};

#endif
