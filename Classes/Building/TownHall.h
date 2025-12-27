#ifndef __TOWNHALL_H__
#define __TOWNHALL_H__

#include "Building/Building.h"

class TownHall : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(TownHall);
    virtual std::string getNextLevelTextureName() override;
    virtual void upgrade() override;
};

#endif