#include "Player.h"
#include <cctype>

Player::Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& theScreen)
    : pos(point), screen(theScreen)
{
    for (int i = 0; i < NUM_KEYS; ++i) {
        keys[i] = the_keys[i];
    }
}


void Player::draw() {
    pos.draw();
}

void Player::move() {
    pos.draw(' ');

    // compute next position
    Point next = pos;
    next.move();             

    // if next is a wall, stay; otherwise actually move
    if (!screen.isWall(next)) {
        pos = next;
    }
    pos.draw();
}

void Player::handleKeyPressed(char key_pressed) {
    size_t index = 0;
    for (char k : keys) {
        if (std::tolower(k) == std::tolower(key_pressed)) {
            pos.setDirection(static_cast<Direction>(index));
            return;
        }
        ++index;
    }
}
