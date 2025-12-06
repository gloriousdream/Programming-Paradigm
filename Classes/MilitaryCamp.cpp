#include "MilitaryCamp.h"
USING_NS_CC;

bool MilitaryCamp::init()
{
    if (!Building::init()) return false;

    setTexture("MilitaryCamp.png");
    level = 1;
    maxHP = 150;
    currentHP = maxHP;
    updateHPBar();

    buildCostGold = 100;
    buildCostHoly = 50;

    upgradeCostGold = 50;
    upgradeCostHoly = 30;

    return true;
}

void MilitaryCamp::upgrade()
{
    level++;
    if (level == 2) {
        setTexture("MilitaryCamp2.png");
        maxHP = 200;
    }
    else if (level == 3) {
        setTexture("MilitaryCamp3.png");
        maxHP = 260;
    }
    currentHP = maxHP;
    updateHPBar();
}
