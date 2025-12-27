#pragma once
#ifndef __GIANT_H__
#define __GIANT_H__

#include "Soldier/Soldier.h"

class Giant : public Soldier
{
public:
    virtual bool init() override;
    CREATE_FUNC(Giant);

    virtual void actionAttack() override;
    virtual void actionWalk() override;
};

#endif