#include "Cannon.h"
#include "Soldier.h" 
#include "SoldierManager.h"

USING_NS_CC;

bool Cannon::init()
{
    // 1. 父类初始化
    if (!Building::init()) return false;

    // 2. 加载图集
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("atlas.plist");

    // 3. 设置 Cannon 本身为底座
    // 使用 setSpriteFrame 会自动把 ContentSize 设置为图片原本的大小
    this->setSpriteFrame("cannon_stand.png");

    // 4. 确保锚点在中心
    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    // 5. 创建炮管
    _barrel = Sprite::createWithSpriteFrameName("cannon01.png");

    if (_barrel) {
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

    // 6. 设置属性 
    level = 1;
    maxHP = 400;
    currentHP = maxHP;

    attackDamage = 20;
    attackRange = 250.0f;
    attackRate = 1.5f;
    _cooldownTimer = 0;

    buildCostGold = 200;
    buildCostHoly = 0;
    upgradeCostGold = 150;
    upgradeCostHoly = 0;

    // 7. 刷新血条 
    this->updateHPBar();

    this->scheduleUpdate();
    return true;
}

void Cannon::update(float dt)
{
    if (_cooldownTimer > 0) {
        _cooldownTimer -= dt;
    }
    // A. 获取所有士兵
    auto& soldiers = SoldierManager::getInstance()->getSoldiers();

    // B. 清理一下死掉的兵
    SoldierManager::getInstance()->cleanDeadSoldiers();

    // C. 寻找最近的敌人
    Soldier* nearestTarget = nullptr;
    float minDistance = this->getAttackRange(); // 初始设为攻击范围，超过这个范围的不看

    for (auto soldier : soldiers)
    {
        // 双重保险：确保兵活着
        if (soldier && soldier->getHP() > 0 && soldier->getParent())
        {
            float dist = this->getPosition().distance(soldier->getPosition());
            if (dist < minDistance)
            {
                minDistance = dist;
                nearestTarget = soldier;
            }
        }
    }

    // D. 如果找到了目标
    if (nearestTarget)
    {
        // 1. 始终让炮口对准目标 
        this->updateBarrelDirection(nearestTarget->getPosition());

        // 2. 尝试开火 (fireAt 内部会检查 _cooldownTimer，所以这里每帧调用没关系)
        this->fireAt(nearestTarget);
    }
}

void Cannon::updateBarrelDirection(Vec2 targetPos)
{
    if (!_barrel) return;

    Vec2 diff = targetPos - this->getPosition();

    // 1. 标准数学角度：逆时针，0度向右，90度向上
    float angleRad = atan2(diff.y, diff.x);
    float angleDeg = CC_RADIANS_TO_DEGREES(angleRad); // -180 到 180

    // 2. 转换为 Cocos 角度 (顺时针，0度向右，90度向下)
    float cocosAngle = -angleDeg;

    // 3. 归一化到 0 ~ 360
    if (cocosAngle < 0) cocosAngle += 360;

    cocosAngle = 360 - cocosAngle;

    // 5. 计算索引
    int index = (int)((cocosAngle + 5) / 10) + 1; // +5 用于四舍五入

    // 循环修正
    if (index > 36) index -= 36;
    if (index < 1) index = 1;

    // 6. 设置图片
    std::string frameName = StringUtils::format("cannon%02d.png", index);
    auto frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    if (!frame) {
        frameName = StringUtils::format("cannon%d.png", index);
        frame = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    }
    if (frame) _barrel->setSpriteFrame(frame);
}

bool Cannon::fireAt(Soldier* target)
{
    if (_cooldownTimer > 0) return false;
    if (!target || target->getHP() <= 0) return false;

    // 给士兵加锁！防止子弹飞的过程中士兵被炸弹炸死导致内存回收
    target->retain();

    // 1. 转向 (完全保持你的逻辑)
    updateBarrelDirection(target->getPosition());

    _cooldownTimer = attackRate;

    // 2. 后坐力动画 (完全保持你的逻辑)
    if (_barrel)
    {
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

    // 3. 发射炮弹 (完全保持你的逻辑)
    auto ball = Sprite::create("CannonBall.png");
    if (!ball)
    {
        // 兜底：画个黑球
        ball = Sprite::create();
        auto draw = DrawNode::create();
        draw->drawDot(Vec2::ZERO, 5, Color4F::BLACK);
        ball->addChild(draw);
    }

    ball->setPosition(this->getPosition());
    if (this->getParent())
    {
        this->getParent()->addChild(ball, 100);
    }

    float dist = this->getPosition().distance(target->getPosition());
    float duration = dist / 600.0f;

    // 提前取出伤害值。
    // 不要捕获 'this'，因为如果加农炮也被摧毁了，this->attackDamage 也会崩。
    int damage = this->attackDamage;

    auto move = MoveTo::create(duration, target->getPosition());

    auto hitCallback = CallFunc::create([target, ball, damage]()
        {

            // 因为 retain 过了，target 指针现在是绝对安全的，不会崩
            // 只需要判断逻辑上是否还活着 (HP > 0) 且在场景中
            if (target && target->getHP() > 0 && target->getParent())
            {
                target->takeDamage(damage);
            }

            ball->removeFromParent();

            // 否则内存永远泄露
            target->release();
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
        attackDamage = 35;
        upgradeCostGold = 300;
    }
    else if (level == 3) {
        maxHP = 750;
        attackDamage = 50;
        upgradeCostGold = 0;
    }

    // 升级后回满血并刷新血条
    currentHP = maxHP;
    updateHPBar();

    CCLOG("Cannon upgraded to Level %d", level);
}