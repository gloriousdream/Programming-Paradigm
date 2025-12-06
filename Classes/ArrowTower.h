#ifndef __ARROW_TOWER_H__
#define __ARROW_TOWER_H__

#include "Building.h"

class ArrowTower : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(ArrowTower);
    virtual void upgrade() override;
};

#endif
