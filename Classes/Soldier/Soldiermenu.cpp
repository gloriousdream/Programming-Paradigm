#include "Soldier/Soldiermenu.h"
#include <string>

USING_NS_CC;

Soldiermenu* Soldiermenu::createMenu()
{
    return Soldiermenu::create();
}

bool Soldiermenu::init()
{
    if (!Layer::init()) return false;

    // 1. 半透明背景
    auto bg = LayerColor::create(Color4B(0, 0, 0, 200)); //稍微深一点，突出前景
    this->addChild(bg);

    // 2. 点击背景关闭菜单 (可选，防止误触)
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* t, Event* e) { return true; };
    listener->onTouchEnded = [=](Touch* t, Event* e)
        {
            // 如果点击的是空白处，可以关闭，这里暂时留空，只拦截点击
        };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, bg);


    // 3. 屏幕中心位置计算
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float centerX = visibleSize.width / 2;
    float centerY = visibleSize.height / 2;

    // 4. 创建四个兵种的控制单元
    // 参数：图片，类型ID，位置(在屏幕上的坐标)

    // 假设排成一排，间距 150
    float startX = centerX - 225; // (150 * 3) / 2

    createSoldierUnit("UI/SoldierMenu/yemanren_select.png", 1, Vec2(startX, centerY));
    createSoldierUnit("UI/SoldierMenu/juren_select.png", 2, Vec2(startX + 150, centerY));
    createSoldierUnit("UI/SoldierMenu/gongjianshou_select.png", 3, Vec2(startX + 300, centerY));
    createSoldierUnit("UI/SoldierMenu/boom_select.png", 4, Vec2(startX + 450, centerY));

    // 添加一个关闭按钮在右上角
    auto closeBtn = MenuItemImage::create("UI/SoldierMenu/CloseNormal.png", "UI/SoldierMenu/CloseSelected.png", [=](Ref*)
        {
            this->removeFromParent();
        });

    auto sysMenu = Menu::create(closeBtn, nullptr);
    sysMenu->setPosition(visibleSize.width - 50, visibleSize.height - 50);
    this->addChild(sysMenu, 10);

    return true;
}

// 创建单个士兵的操作面板
void Soldiermenu::createSoldierUnit(const std::string& imgName, int type, Vec2 pos)
{
    // 1. 容器 Node
    // 创建一个 Node 放在指定位置，所有元素都加在这个 Node 上
    auto container = Node::create();
    container->setPosition(pos);
    this->addChild(container);

    // 2. 士兵图标 (静态图片) 
    auto sprite = Sprite::create(imgName);
    if (sprite)
    {
        container->addChild(sprite);
    }
    else
    {
        // 防崩溃：如果图片没找到，显示一个色块
        auto node = LayerColor::create(Color4B::GRAY, 64, 64);
        node->setPosition(-32, -32);
        container->addChild(node);
    }

    // 第一阶段：[训练] 按钮 
    auto lblTrain = Label::createWithSystemFont("Train", "Arial", 24);
    lblTrain->setColor(Color3B::GREEN);

    auto btnTrain = MenuItemLabel::create(lblTrain);
    btnTrain->setPosition(Vec2(0, -60)); // 在图片下方

    // 4. 第二阶段：[ - ] [数量] [ + ] [确定] 
    // 这部分初始是隐藏的

    // 数字显示 Label
    auto countLabel = Label::createWithSystemFont("1", "Arial", 24);
    countLabel->setPosition(Vec2(0, -60));
    countLabel->setVisible(false);
    container->addChild(countLabel);

    // [-] 按钮
    auto lblMinus = Label::createWithSystemFont("-", "Arial", 30);
    lblMinus->setColor(Color3B::RED);
    auto btnMinus = MenuItemLabel::create(lblMinus, [=](Ref*)
        {
            // 获取当前数量
            int c = std::stoi(countLabel->getString());
            if (c > 1)
            {
                c--;
                countLabel->setString(std::to_string(c));
            }
        });
    btnMinus->setPosition(Vec2(-40, -60));

    // [+] 按钮
    auto lblPlus = Label::createWithSystemFont("+", "Arial", 30);
    lblPlus->setColor(Color3B::GREEN);
    auto btnPlus = MenuItemLabel::create(lblPlus, [=](Ref*)
        {
            int c = std::stoi(countLabel->getString());
            if (c < 99)
            { // 限制最大数量
                c++;
                countLabel->setString(std::to_string(c));
            }
        });
    btnPlus->setPosition(Vec2(40, -60));

    // [OK] 确定按钮 (放在更下面)
    auto lblOk = Label::createWithSystemFont("OK", "Arial", 20);
    lblOk->setColor(Color3B::YELLOW);
    auto btnOk = MenuItemLabel::create(lblOk, [=](Ref*)
        {
            // 读取最终数量
            int finalCount = std::stoi(countLabel->getString());

            // 触发外部回调
            if (onTrainSoldier)
            {
                onTrainSoldier(type, finalCount);
            }

            // 关闭菜单
            this->removeFromParent();
        });
    btnOk->setPosition(Vec2(0, -100));

    // 5. 组装菜单与逻辑连接

    // 将第二阶段的按钮放在一个 Menu 里，初始设为隐藏
    auto menuPhase2 = Menu::create(btnMinus, btnPlus, btnOk, nullptr);
    menuPhase2->setPosition(Vec2::ZERO); // 相对 container 的 (0,0)
    menuPhase2->setVisible(false);
    container->addChild(menuPhase2);

    // 设置第一阶段 [训练] 按钮的回调
    btnTrain->setCallback([=](Ref*)
        {
            // 点击 [Train] 后：
            // 1. 隐藏自己 (Train按钮)
            btnTrain->setVisible(false); // 注意：Menu item visible false 只是不显示，但为了保险通常移除

            // 2. 显示第二阶段界面
            countLabel->setVisible(true);
            menuPhase2->setVisible(true);
        });

    // 创建第一阶段菜单
    auto menuPhase1 = Menu::create(btnTrain, nullptr);
    menuPhase1->setPosition(Vec2::ZERO);
    container->addChild(menuPhase1);
}