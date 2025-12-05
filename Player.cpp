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
	auto found = res.first;
	auto itemPos = res.second;
    if(inventory == nullptr && found) {
        inventory = screen.getItem(itemPos);
		screen.changePixelInRoom(itemPos, ' ');
	}
}

void Player::dispose() {
    if (inventory != nullptr) {
        (*inventory).drop();
		auto res = pos.PlaceToDrop(screen, 1);
		auto found = res.first;
		auto itemPos = res.second;
        if (found) {
            auto itempos  = inventory->getPos();
            itempos = pos;
			screen.changePixelInCurrBoard(itemPos, inventory->getCh());
            inventory = nullptr;
        }
    }
}




void Player::move(){
    pos.draw(' ');

    // compute next position
    Point next = pos;
    next.move();             

    // if next is a wall, stay; otherwise actually move
    if (!(screen.isWall(next) || screen.isItem(next))) {
        pos = next;
    }
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
