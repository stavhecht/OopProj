#include "Player.h"
#include <cctype>

Player::Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& room)
    : pos(point), screen(room){
    for (int i = 0; i < NUM_KEYS; ++i) {
        keys[i] = the_keys[i];
    }
}


void Player::draw() {
    pos.draw();
}


void Player::pickUp() {
    auto res = pos.ItemInRadios(screen, 1);
    bool found = res.first;
    Point itemPos = res.second;

    if (!found || inventory != nullptr)
        return;

    // get item from screen
    CollectableItems* item = dynamic_cast<CollectableItems*>(screen.getItem(itemPos));
    if (!item)
        return;

    inventory = item;

    // remove from room 
    screen.changePixelInRoom(itemPos, ' ');

    // let item react
    inventory->onPickUp(*this, screen);

    screen.printRoom();
}


void Player::dispose() {
    if (!inventory)
        return;

    auto res = pos.PlaceToDrop(screen, 1);
    bool found = res.first;
    if (!found)
        return;

    Point dropPos = res.second;

    // copy appearance (glyph + color) from the inventory into dropPos
    // this uses Point::operator=(const Item&), which preserves dropPos coordinates
    dropPos = *inventory;

    inventory->setPos(dropPos);
    inventory->onDrop(*this, screen);

    // return ownership to screen so it becomes an active item again
    screen.addItem(inventory);

    inventory = nullptr;
    screen.printRoom();
}

CollectableItems* Player::takeInventory() {
    CollectableItems* tmp = inventory;
    inventory = nullptr;
    return tmp;
}




void Player::move() {
    // erase current
    pos.draw(' ');

    // compute next
    Point next = pos;
    next.move();

    // block on walls/doors/items (like wall collision)
    if (screen.isWall(next) || screen.isDoor(next) || screen.isItem(next)) {
        // redraw current position and don't move
        pos.draw();
        return;
    }

    pos = next;
    pos.draw();
}

void Player::handleKeyPressed(char key_pressed) {
    size_t index = 0;
    for (char k : keys) {
        if (std::tolower(k) == std::tolower(key_pressed)) {
            if (k == 'o' || k == 'e') {
                if (inventory == nullptr)
                    pickUp();
                else
                    dispose();
            }
            else {
                pos.setDirection(static_cast<Direction>(index));
                return;
            }
        }
        ++index;
    }
}
