#include "Building/GoldStage.h"

USING_NS_CC;
std::string GoldStage::getNextLevelTextureName()
{
    int nextLv = level + 1;

    if (nextLv == 2) return "gold_stage_01.png"; // 推测命名
    if (nextLv == 3) return "gold_stage_01.png"; // 推测命名

    return "";
}
bool GoldStage::init()
{
    if (!Building::init()) return false;

    // 1. 初始化属性
    this->buildCostGold = 100;
    this->buildCostHoly = 0;

    // 2. 设置图片
    this->setTexture("gold_stage_01.png");


    this->setScale(0.5f);

    this->setAnchorPoint(Vec2(0.5, 0.5));
    this->maxHP = 1000;
    this->currentHP = maxHP;

    // 3. 让父类去负责刷新血条位置
    // 父类会根据当前的 Texture 大小自动计算血条位置
    this->updateHPBar();

    return true;
}
void GoldStage::takeDamage(int damage)
{
    // 1. 先调用父类扣血逻辑 (处理血条、死亡等)
    Building::takeDamage(damage);

    // 2. 计算掉落金币数量
    // 逻辑：每次被打掉多少血，就掉落多少金币 (你可以乘以倍率，比如 * 2)
    int lootAmount = damage * 2;

    // 3. 发送 "LOOT_GOLD_EVENT" 事件
    // 这里必须和 FightScene 里的监听字符串一模一样
    EventCustom event("LOOT_GOLD_EVENT");
    event.setUserData(&lootAmount); // 把数量传过去
    _eventDispatcher->dispatchEvent(&event);

}
void GoldStage::upgrade()
{
    // 升级逻辑示例
    level++;
    maxHP += 200;
    CCLOG("GoldStage upgraded to level %d", level);
}

void GoldStage::updateVisuals(int currentGold, int maxGold)
{
    if (maxGold <= 0) maxGold = 1; // 防止除零

    // 计算当前金币百分比
    float percent = static_cast<float>(currentGold) / static_cast<float>(maxGold);
    // 限制百分比在 0.0 - 1.0 之间
    if (percent > 1.0f) percent = 1.0f;
    if (percent < 0.0f) percent = 0.0f;

    // 决定显示第几张图 (1-5)
    int stage = 1;
    if (percent >= 0.8f)      stage = 5; // >80% 满
    else if (percent >= 0.6f) stage = 4; // >60%
    else if (percent >= 0.4f) stage = 3; // >40%
    else if (percent >= 0.2f) stage = 2; // >20%
    else                      stage = 1; // 空

    // 只有状态改变时才更换贴图，节省性能
    if (stage != _visualLevel)
    {
        _visualLevel = stage;
        std::string textureName = StringUtils::format("gold_stage_%02d.png", stage);

        // 切换图片
        this->setTexture(textureName);

       
    }
}