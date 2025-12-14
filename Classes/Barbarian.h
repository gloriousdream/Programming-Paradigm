#pragma once
#ifndef __BARBARIAN_H__
#define __BARBARIAN_H__

#include "Soldier.h"

class Barbarian : public Soldier
{
public:
    virtual bool init() override;
    CREATE_FUNC(Barbarian);

    virtual void actionAttack() override;
    virtual void actionWalk() override;
};

#endif