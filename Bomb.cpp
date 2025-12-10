#include "Bomb.h"
#include "Screen.h"
#include "Player.h"

// Perform explosion: clear map characters, remove live items inside radius.
void Bomb::triggerBomb(Screen& screen, const Point& center) {
    int radius = 3;
    for (int dy = -radius; dy <= radius; ++dy) {
        for (int dx = -radius; dx <= radius; ++dx) {
            int distSq = dx * dx + dy * dy;
            if (distSq <= radius * radius) {
                int x = center.getX() + dx;
                int y = center.getY() + dy;
                if (x >= 0 && x < Screen::MAX_X && y >= 0 && y < Screen::MAX_Y) {
                    Point p(x, y);
                    // clear the template room representation
                    screen.changePixelInRoom(p, ' ');

                    // if there is a live Item at this point, remove it
                    screen.removeItemAt(p);
                }
            }
        }
    }

    // Hide any registered players who are within blast radius.
    screen.hidePlayersInRadius(center, radius);

    // Redraw room after explosion
    screen.printRoom();
}

void Bomb::onDrop(Player& /*player*/, Screen& /*screen*/) {
    // nothing here; arming handled externally
    picked = false;
}

// Arm a bomb to explode after `waiting` game-loop ticks. No duplicate entries.
void Bomb::arm(int ticks) {
    if (ticks <= 0) return;
    armed = true;
    remainingTicks = ticks;
}

// Update all pending bombs: decrement ticks, explode those that reach zero.
// This function must be called once per game-loop iteration (tick).
bool Bomb::tick() {
    if (!armed) return false;
    --remainingTicks;
    if (remainingTicks <= 0) {
        armed = false;
        remainingTicks = 0;
        return true;
    }
    return false;
}