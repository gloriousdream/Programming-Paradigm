#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    CREATE_FUNC(GameScene);

private:
    bool placeMode = false;
    int selectedBuildingType = 0;

    void onBuildButtonPressed();
    void enablePlaceMode(int type);
    void onMapClicked(cocos2d::Vec2 pos);
};

#endif
