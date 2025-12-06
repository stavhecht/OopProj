#pragma once
#include "Item.h"

class CollectableItems : public Item {
    bool picked = false;

public:
    CollectableItems(const Point& p, char c, Color col)
        : Item(p, c, col) {
    }

    bool isPicked() const { return picked; }

    virtual void onPickUp(Player& player, Screen& screen) override {
        picked = true;
        // e.g. torch/key: update player inventory flags here
    }

    virtual void onDrop(Player& player, Screen& screen) override {
        picked = false;
        // maybe nothing else
    }
};
