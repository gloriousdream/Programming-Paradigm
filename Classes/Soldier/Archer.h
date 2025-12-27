#pragma once
#ifndef __ARCHER_H__
#define __ARCHER_H__

#include "Soldier/Soldier.h"

class Archer : public Soldier
{
public:
    virtual bool init() override;
    CREATE_FUNC(Archer);

    virtual void actionAttack() override;
    virtual void actionWalk() override;
};

#endif