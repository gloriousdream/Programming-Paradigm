#include "StartBackground.h"
#include "GameScene.h"

USING_NS_CC;

Scene* StartBackground::createScene()
{
    return StartBackground::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in StartBackgroundScene.cpp\n");
}

// on "init" you need to initialize your instance
bool StartBackground::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto screensize = Director::getInstance()->getVisibleSize();

    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    auto startItem = MenuItemImage::create(
        "GetStart.png",      // 按钮的图片
        "GetStart.png",      // 可以使用同一张（如果没有按下态）
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


void StartBackground::menuStartGameCallback(Ref* pSender)
{
    auto nextScene = GameScene::createScene();
    Director::getInstance()->replaceScene(TransitionFade::create(1.0f, nextScene));
}
