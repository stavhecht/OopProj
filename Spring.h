#pragma once
#include "SteppedOnItems.h"

class Spring : public SteppedOnItems {
public:
    Spring(const Point& p, char c = '#', Color col = Color::Gray)
        : SteppedOnItems(p, c, col) {}

    void onStep(Player& player, Screen& screen) override;

    virtual Item* clone() const override { return new Spring(*this); }
};

