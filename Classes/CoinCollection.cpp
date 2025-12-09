#include "CoinCollection.h"
USING_NS_CC;

bool CoinCollection::init()
{
    // 1. 初始化基类
    if (!Building::init()) return false;

    // 2. 设置初始贴图和属性
    setTexture("CoinCollection.png"); // 等级1图片
    level = 1;
    maxHP = 150;        // 假设血量比水库稍高一点
    currentHP = maxHP;
    updateHPBar();

    // 3. 设置建造消耗
    buildCostGold = 20;
    buildCostHoly = 80;

    // 4. 设置升级消耗
    upgradeCostGold = 40;
    upgradeCostHoly = 100;

    return true;
}

void CoinCollection::upgrade()
{
    level++;
    if (level == 2) {
        setTexture("CoinCollection2.png");
        maxHP = 200;
        // 可以在这里修改下一级的升级消耗
        upgradeCostGold = 80;
        upgradeCostHoly = 150;
    }
    else if (level == 3) {
        setTexture("CoinCollection3.png");
        maxHP = 300;
    }

    // 升级后补满血并更新血条
    currentHP = maxHP;
    updateHPBar();
}