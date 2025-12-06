#ifndef __MILITARY_CAMP_H__
#define __MILITARY_CAMP_H__

#include "Building.h"

class MilitaryCamp : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(MilitaryCamp);
    virtual void upgrade() override;
};

#endif
