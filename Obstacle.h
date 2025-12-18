#pragma once
#include "SteppedOnItems.h"
#include <vector>

class Obstacle : public SteppedOnItems {
public:
    Obstacle(const Point& p, char c = '*', Color col = Color::White)
        : SteppedOnItems(p, c, col) {
    }

    // Attempt to react when a player steps adjacent.
    // We override to allow pushing when the player is adjacent and pushing toward the obstacle.
    void onStep(Player& player, Screen& screen) override;

    // Helpers:
    // Collect connected obstacle cells (orthogonal adjacency) starting from `start`.
    static std::vector<Obstacle*> collectGroup(Screen& screen, const Point& start);

    // Returns true if a group containing `start` can be pushed by `force` in `dir`.
    static bool canPushGroup(Screen& screen, const Point& start, Direction dir, int force);

    // Push the group containing `start` one cell in `dir`. Returns true on success.
    static bool pushGroup(Screen& screen, const Point& start, Direction dir);

    virtual Item* clone() const override { return new Obstacle(*this); }
};

