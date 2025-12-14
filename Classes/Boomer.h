#pragma once
#ifndef __BOMBER_H__
#define __BOMBER_H__

#include "Soldier.h"

class Bomber : public Soldier
{
public:
    virtual bool init() override;
    CREATE_FUNC(Bomber);

    virtual void actionAttack() override;
    virtual void actionWalk() override;
};

#endif