// SteppedOnItems.h
#pragma once
#include "Item.h"

class SteppedOnItems : public Item {
public:
    SteppedOnItems(const Point& p, char c, Color col)
        : Item(p, c, col) {}

    virtual void onStep(Player& player, Screen& screen)  {
        // e.g. switch, riddle, spring, bomb trigger, etc.
    }
};
