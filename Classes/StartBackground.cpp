#include "StartBackground.h"
#include "GameScene.h"

USING_NS_CC;

Scene* StartBackground::createScene()
{
    return StartBackground::create();
}

static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in StartBackgroundScene.cpp\n");
}

bool StartBackground::init()
{
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto screensize = Director::getInstance()->getVisibleSize();

    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 加入图片

    auto startItem = MenuItemImage::create(
        "GetStart.png",      // 开始按钮的图片
        "GetStart.png",
        CC_CALLBACK_1(StartBackground::menuStartGameCallback, this)
    );

    float x = origin.x + screensize.width - startItem->getContentSize().width / 2;
    float y = origin.y + startItem->getContentSize().height / 2;
    startItem->setPosition(Vec2(x, y));

    auto menu = Menu::create(startItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // 创建背景图片
    auto sprite = Sprite::create("StartBackground.jpg");

    sprite->setPosition(Vec2(screensize.width / 2 + origin.x, screensize.height / 2 + origin.y));

    this->addChild(sprite, 0);

    return true;
}


// StartBackground.cpp

void StartBackground::menuStartGameCallback(Ref* pSender)
{
    

    // 切换场景
    auto scene = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(1.0f, scene));
}