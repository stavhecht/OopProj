#pragma once
#include "SteppedOnItems.h"


class Door : public SteppedOnItems {
    bool opened = false;

public:
    Door(const Point& p, char c, Color col = Color::White)
        : SteppedOnItems(p, c, col), opened(false) {
    }

    Door(const Door&) = default;
    Door& operator=(const Door&) = default;

    bool isOpen() const { return opened; }
    void open() { opened = true; }
    void onStep(Player& player, Screen& screen) override;

    virtual Item* clone() const override { return new Door(*this); }
};
