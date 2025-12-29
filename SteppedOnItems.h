// SteppedOnItems.h
#pragma once
#include "Item.h"

class SteppedOnItems : public Item {
public:
    SteppedOnItems(const Point& p, char c, Color col)
        : Item(p, c, col) {}

    SteppedOnItems(const SteppedOnItems&) = default;
    SteppedOnItems& operator=(const SteppedOnItems&) = default;

    virtual void onStep(Player& player, Screen& screen)  {
        // individual for switch, riddle, spring, bomb trigger, etc.
    }

    virtual Item* clone() const override { return new SteppedOnItems(*this); }
};
