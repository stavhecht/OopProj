#pragma once
#include "Item.h"

class CollectableItems : public Item {
protected:
    bool picked = false;

public:
    CollectableItems(const Point& p, char c, Color col)
        : Item(p, c, col) {
    }

    CollectableItems(const CollectableItems&) = default;
    CollectableItems& operator=(const CollectableItems&) = default;

    bool isPicked() const { return picked; }

    virtual void onPickUp(Player& player, Screen& screen) override {
        picked = true;
        // e.g. torch/key: update player inventory flags here
    }

    virtual void onDrop(Player& player, Screen& screen) override {
        picked = false;
        // maybe nothing else
    }

    virtual Item* clone() const override { return new CollectableItems(*this); }
};
