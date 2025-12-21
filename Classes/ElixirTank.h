#pragma once
#ifndef __ELIXIR_TANK_H__
#define __ELIXIR_TANK_H__

#include "Building.h"

class ElixirTank : public Building
{
public:
    virtual bool init() override;
    CREATE_FUNC(ElixirTank);
    virtual std::string getNextLevelTextureName() override;

    virtual void upgrade() override;
    virtual void takeDamage(int damage) override;
    // 根据当前圣水数量更新外观 (01-05)
    void updateVisuals(int currentHoly, int maxHoly);

private:
    int _visualLevel = -1;
};

#endif