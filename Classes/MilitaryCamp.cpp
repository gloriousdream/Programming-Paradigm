#include "MilitaryCamp.h"
USING_NS_CC;

bool MilitaryCamp::init()
{
    if (!Building::init()) return false;
    level = 1;
    setTexture("MilitaryCamp.png");
    return true;
}

void MilitaryCamp::upgrade()
{
    level++;
    if (level == 2) setTexture("MilitaryCamp.png");
    else if (level == 3) setTexture("MilitaryCamp.png");
}