
#define __GAME_SCENE_H__

#include "cocos2d.h"

class GameScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);
    virtual void onEnter() override;
    void onBuildingClicked(cocos2d::Sprite* building);
    void showUpgradeButton(cocos2d::Sprite* building);
    void updateResourceDisplay();
    // 专门显示兵营的操作菜单（升级 + 造兵）
    void showMilitaryOptions(cocos2d::Sprite* building);
    static int getGlobalGold();
    static int getGlobalHolyWater();
    static void addGlobalResources(int addGold, int addHoly);
    void showTrainMenu(cocos2d::Sprite* building);
    void showTrainAmountMenu(int soldierType);


private:
    bool placeModebuild = false;
    bool placeModesoldier = false;
    int selectedType = 0;
    static int gold;
    static int holyWater;
    // 当前显示升级菜单的建筑
    cocos2d::Sprite* currentBuildingMenu = nullptr;

    // 用于记录当前打开的弹窗（升级菜单、造兵菜单等）
    cocos2d::Node* currentPopup = nullptr;

    cocos2d::Sprite* ghostSprite = nullptr;

    // 关闭当前弹窗的辅助函数
    void closeCurrentPopup();

    // 玩家资源

    int population = 0;

    cocos2d::Label* goldLabel = nullptr;
    cocos2d::Label* waterLabel = nullptr;
    cocos2d::Label* populationLabel = nullptr;
    void onFightpushed();
    void onSoldierpushed();
    void onBuildButtonPressed();

    template<typename T>
    void enablePlaceMode(int type, T menu);
    void onMapClicked(cocos2d::Vec2 pos);

    // 记录当前难度
    const int TAG_DIFFICULTY_MENU = 8888;

};


