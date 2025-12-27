#ifndef __ARROW_TOWER_H__
#define __ARROW_TOWER_H__

#include "Building/Building.h"

class ArrowTower : public Building
{
public:
    virtual bool init() override;
    virtual std::string getNextLevelTextureName() override;

    CREATE_FUNC(ArrowTower);
    virtual void upgrade() override;
};

#endif