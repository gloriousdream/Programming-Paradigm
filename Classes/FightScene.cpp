#include "FightScene.h"
#include "GameScene.h" // 如果你想加返回按钮，可能需要引用 GameScene

USING_NS_CC;

Scene* FightScene::createScene(int difficulty)
{
    return FightScene::create(difficulty);
}

FightScene* FightScene::create(int difficulty)
{
    FightScene* pRet = new(std::nothrow) FightScene();
    if (pRet && pRet->initWithDifficulty(difficulty))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

// 标准 init，先保留
bool FightScene::init()
{
    if (!Scene::init()) return false;
    return true;
}

bool FightScene::initWithDifficulty(int difficulty)
{
    // 1. 先调用父类的 init
    if (!Scene::init()) return false;

    _difficulty = difficulty;

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 2. 加载背景
    auto bg = Sprite::create("GrassBackground.png");
    if (bg) {
        bg->setAnchorPoint(Vec2::ZERO);
        bg->setPosition(origin);
        this->addChild(bg, 0);
    }

    // 3. 根据难度显示不同的文字
    std::string diffText = "Mode: Unknown";
    if (_difficulty == 1) diffText = "Mode: EASY";
    else if (_difficulty == 2) diffText = "Mode: MIDDLE";
    else if (_difficulty == 3) diffText = "Mode: HARD";

    auto label = Label::createWithSystemFont(diffText, "Arial", 48);
    label->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - 100));
    label->setColor(Color3B::RED);
    this->addChild(label, 10);

    // 4. 返回按钮

    MenuItem* closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        [](Ref* sender) {
            // 不仅仅是切换场景，而是“弹出”当前场景
            // 这样会自动回到之前的 GameScene，且状态完全保留
            Director::getInstance()->popScene();
        });

    // 兜底逻辑：如果图片加载失败
    if (closeItem == nullptr || closeItem->getContentSize().width == 0)
    {
        auto lbl = Label::createWithSystemFont("Back", "Arial", 30);

        closeItem = MenuItemLabel::create(lbl, [](Ref*) {
            Director::getInstance()->popScene();
            });
    }

    closeItem->setPosition(Vec2(origin.x + visibleSize.width - 50, origin.y + 50));

    auto menu = Menu::create(closeItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 20);

    return true;
}