#include "Building/Boom.h"

USING_NS_CC;

bool Boom::init()
{
    if (!Building::init()) return false;

    // 1. 默认状态：家园模式（可见、不透明）
    setTexture("boom1.png");
    this->setOpacity(255);
    this->setVisible(true);

    // 2. 基础属性
    level = 1;
    maxHP = 200; currentHP = maxHP; updateHPBar();
    buildCostGold = 150; buildCostHoly = 0;
    upgradeCostGold = 100; upgradeCostHoly = 0;

    // 3. 初始化战斗参数
    _triggerRange = 60.0f;      // 敌人靠近 60 像素时触发
    _explosionRadius = 120.0f;  // 爆炸波及 120 像素
    _damage = 500;              // 造成 500 点伤害

    return true;
}

std::string Boom::getNextLevelTextureName()
{
    if (level == 1) return "boom2.png";
    if (level == 2) return "boom3.png";
    return "boom1.png";
}

void Boom::upgrade()
{
    level++;
    maxHP += 50;
    _damage += 200; // 升级提升伤害
    currentHP = maxHP;
    updateHPBar();
}

// 战斗模式：变身隐形
void Boom::setToFightMode()
{
    // 将透明度设为 0 完全隐形，或者设为 20 (半透明) 方便调试
    this->setOpacity(255);

    // 隐藏血条 (如果有的话)
    if (hpBar) hpBar->setVisible(false);
}

// 爆炸特效
void Boom::playExplodeEffect()
{
    // 1. 显形
    this->setOpacity(255);

    // 2. 播放动画：瞬间变大 -> 渐隐 -> 移除
    auto scaleUp = ScaleTo::create(0.1f, 2.0f);
    auto fadeOut = FadeOut::create(0.5f);
    auto remove = RemoveSelf::create();

    this->runAction(Sequence::create(scaleUp, fadeOut, remove, nullptr));
}