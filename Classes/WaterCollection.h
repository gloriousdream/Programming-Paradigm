#ifndef __WATER_COLLECTION_H__
#define __WATER_COLLECTION_H__

#include "Building.h"

class WaterCollection : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(WaterCollection);
    virtual void upgrade() override;
};

#endif