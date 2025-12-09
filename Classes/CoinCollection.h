#ifndef __COIN_COLLECTION_H__
#define __COIN_COLLECTION_H__

#include "Building.h"

class CoinCollection : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(CoinCollection);
    virtual void upgrade() override;
};

#endif