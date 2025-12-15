#include "Cannon.h"
#include "Soldier.h" 

USING_NS_CC;

bool Cannon::init()
{
    // 1. 父类初始化
    if (!Building::init()) return false;

    // 2. 加载图集
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("atlas.plist");

    // 3. 【修复变形】设置 Cannon 本身为底座
    // 使用 setSpriteFrame 会自动把 ContentSize 设置为图片原本的大小
    // 这样就不会强制拉伸成 128x128 了
    this->setSpriteFrame("cannon_stand.png");

    // 4. 【修复吸附偏移】确保锚点在中心
    // BuildingManager 是根据中心点计算吸附的，如果锚点不对，位置就会偏
    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    // 5. 创建炮管
    _barrel = Sprite::createWithSpriteFrameName("cannon01.png");

    if (_barrel) {
        // 【修复对齐】
        // 获取底座当前的实际大小
        Size standSize = this->getContentSize();

        // 确保炮管锚点也是中心，方便旋转
        _barrel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

        // 将炮管放在底座的几何中心，并向上偏移 10 像素
        _barrel->setPosition(standSize.width / 2, standSize.height / 2 + 10);

        this->addChild(_barrel, 1); // Z轴为1，确保盖在底座上面
    }
    else {
        CCLOG("Error: 找不到 cannon01.png");
    }

    // 6. 设置属性 (保持你原来的数值)
    level = 1;
    maxHP = 400;
    currentHP = maxHP;

    attackDamage = 60;
    attackRange = 250.0f;
    attackRate = 1.5f;
    _cooldownTimer = 0;

    buildCostGold = 200;
    buildCostHoly = 0;
    upgradeCostGold = 150;
    upgradeCostHoly = 0;

    // 7. 刷新血条 (因为尺寸变了，需要重新计算位置)
    this->updateHPBar();

    this->scheduleUpdate();
    return true;
}

void Cannon::update(float dt)
{
    if (_cooldownTimer > 0) {
        _cooldownTimer -= dt;
    }
}

void Cannon::updateBarrelDirection(Vec2 targetPos)
{
    if (!_barrel) return;

    Vec2 myPos = this->getPosition();
    Vec2 diff = targetPos - myPos;

    // 计算角度
    float angleRad = atan2(diff.y, diff.x);
    float angleDeg = CC_RADIANS_TO_DEGREES(-angleRad);

    // 360度切图逻辑 (1 ~ 36)
    int totalAngle = (int)(angleDeg + 360 + 5);
    int index = (totalAngle % 360) / 10 + 1;

    if (index > 36) index = 1;
    if (index < 1) index = 36;

    // 拼接文件名 cannon01.png ~ cannon36.png
    std::string frameName = StringUtils::format("cannon%02d.png", index);

    // 切换炮管贴图
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    if (frame) {
        _barrel->setSpriteFrame(frame);
    }
}

// ==========================================
// 【绝对没有修改】保留你原本的开火逻辑
// ==========================================
bool Cannon::fireAt(Soldier* target)
{
    if (_cooldownTimer > 0) return false;
    if (!target || target->getHP() <= 0) return false;

    // 1. 转向
    updateBarrelDirection(target->getPosition());

    _cooldownTimer = attackRate;

    // 2. 后坐力动画
    if (_barrel) {
        Vec2 dir = (target->getPosition() - this->getPosition()).getNormalized();
        // 初始位置参考 init 里的设置 (注意保持一致)
        Vec2 originalPos = Vec2(getContentSize().width / 2, getContentSize().height / 2 + 10);
        Vec2 recoilPos = originalPos - (dir * 5.0f);

        _barrel->runAction(Sequence::create(
            MoveTo::create(0.05f, recoilPos),
            MoveTo::create(0.1f, originalPos),
            nullptr
        ));
    }

    // 3. 发射炮弹 (原样逻辑)
    auto ball = Sprite::create("CannonBall.png");
    if (!ball) {
        // 兜底：画个黑球
        ball = Sprite::create();
        auto draw = DrawNode::create();
        draw->drawDot(Vec2::ZERO, 5, Color4F::BLACK);
        ball->addChild(draw);
    }

    ball->setPosition(this->getPosition());
    if (this->getParent()) {
        this->getParent()->addChild(ball, 100);
    }

    float dist = this->getPosition().distance(target->getPosition());
    float duration = dist / 600.0f;

    auto move = MoveTo::create(duration, target->getPosition());
    auto hitCallback = CallFunc::create([target, ball, this]() {
        if (target && target->getParent()) {
            target->takeDamage(this->attackDamage);
        }
        ball->removeFromParent();
        });

    ball->runAction(Sequence::create(move, hitCallback, nullptr));

    return true;
}

void Cannon::upgrade()
{
    if (level >= 3) return;
    level++;

    // 升级逻辑：提升属性
    if (level == 2) {
        maxHP = 550;
        attackDamage = 90;
        upgradeCostGold = 300;
    }
    else if (level == 3) {
        maxHP = 750;
        attackDamage = 130;
        upgradeCostGold = 0;
    }

    // 升级后回满血并刷新血条
    currentHP = maxHP;
    updateHPBar();

    CCLOG("Cannon upgraded to Level %d", level);
}