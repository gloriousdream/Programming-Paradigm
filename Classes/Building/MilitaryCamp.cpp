#include "Building/MilitaryCamp.h"
USING_NS_CC;

bool MilitaryCamp::init()
{
    if (!Building::init()) return false;

    setTexture("Buildings/MilitaryCamp/MilitaryCamp.png");
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
std::string MilitaryCamp::getNextLevelTextureName()
{
    int nextLv = level + 1;

    if (nextLv == 2) return "Buildings/MilitaryCamp/MilitaryCamp2.png";
    if (nextLv == 3) return "Buildings/MilitaryCamp/MilitaryCamp3.png";

    return "";
}
void MilitaryCamp::upgrade()
{
    level++;
    if (level == 2)
    {
        setTexture("Buildings/MilitaryCamp/MilitaryCamp2.png");
        maxHP = 200;
    }
    else if (level == 3)
    {
        setTexture("Buildings/MilitaryCamp/MilitaryCamp3.png");
        maxHP = 260;
    }
    currentHP = maxHP;
    updateHPBar();
}