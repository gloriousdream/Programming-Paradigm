#include "ElixirTank.h"

USING_NS_CC;
std::string ElixirTank::getNextLevelTextureName()
{
    int nextLv = level + 1;

    if (nextLv == 2) return "elixir_tank_01.png"; // 推测命名
    if (nextLv == 3) return "elixir_tank_01.png"; // 推测命名

    return "";
}
bool ElixirTank::init()
{
    if (!Building::init()) return false;

    // 1. 初始化属性
    this->buildCostGold = 100;
    this->buildCostHoly = 0;

    // 2. 设置图片
    this->setTexture("elixir_tank_01.png");


    this->setScale(0.5f);

    this->setAnchorPoint(Vec2(0.5f, 0.5f));
    this->maxHP = 1000;
    this->currentHP = maxHP;

    // 3. 让父类刷新血条
    this->updateHPBar();

    return true;
}
void ElixirTank::upgrade()
{
    // 升级逻辑示例
    level++;
    maxHP += 200;
}

void ElixirTank::takeDamage(int damage)
{
    // 1. 父类扣血
    Building::takeDamage(damage);

    // 2. 计算掉落圣水数量
    int lootAmount = damage * 2;

    // 3. 发送 "LOOT_HOLY_EVENT" 事件
    EventCustom event("LOOT_HOLY_EVENT");
    event.setUserData(&lootAmount);
    _eventDispatcher->dispatchEvent(&event);
}
void ElixirTank::updateVisuals(int currentHoly, int maxHoly)
{
    if (maxHoly <= 0) maxHoly = 1;

    // 计算圣水百分比
    float percent = static_cast<float>(currentHoly) / static_cast<float>(maxHoly);
    if (percent > 1.0f) percent = 1.0f;
    if (percent < 0.0f) percent = 0.0f;

    // 决定显示第几张图 (1-5)
    int stage = 1;
    if (percent >= 0.8f)      stage = 5; // 满
    else if (percent >= 0.6f) stage = 4;
    else if (percent >= 0.4f) stage = 3;
    else if (percent >= 0.2f) stage = 2;
    else                      stage = 1; // 空

    // 只有状态改变时才换图
    if (stage != _visualLevel)
    {
        _visualLevel = stage;
        std::string textureName = StringUtils::format("elixir_tank_%02d.png", stage);

        this->setTexture(textureName);

        // 保持锚点在正中心
        this->setAnchorPoint(Vec2(0.5f, 0.5f));
    }
}