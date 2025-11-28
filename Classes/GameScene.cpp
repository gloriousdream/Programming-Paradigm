#include "GameScene.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    return GameScene::create();
}

bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //////////////////////////////
    // 2. Ìí¼Ó±³¾° PlantBackground.png
    auto bg = Sprite::create("PlantBackground.png");

    if (bg == nullptr)
    {
        printf("Error loading 'PlantBackground.png'\n");
    }
    else
    {
        bg->setPosition(Vec2(
            visibleSize.width / 2 + origin.x,
            visibleSize.height / 2 + origin.y
        ));

        this->addChild(bg, 0);
    }

    return true;
}
