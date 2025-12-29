#pragma once

#include "CollectableItems.h"

class Screen;
class Player;

class Bomb : public CollectableItems {
public:
    Bomb(const Point& p, char c = '@', Color col = Color::Red)
        : CollectableItems(p, c, col) {}

    Bomb(const Bomb&) = default;
    Bomb& operator=(const Bomb&) = default;

    // Called when a player drops the bomb - override kept but arming handled by Player::dispose
    virtual void onDrop(Player& player, Screen& screen) override;

    // Perform explosion centered on 'center'
    static void triggerBomb(Screen& screen, const Point& center);

    // Arm this bomb to explode after `ticks` game-loop updates.
    void arm(int ticks);

    // Called by the world each tick. Returns true if the bomb reached zero and should explode now.
    bool tick();

    bool isArmed() const { return armed; }

    virtual Item* clone() const override { return new Bomb(*this); }

private:
    bool armed = false;
    int remainingTicks = 0;
};

