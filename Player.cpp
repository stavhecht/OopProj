#include "Player.h"
#include "Bomb.h"
#include "Torch.h"
#include <cctype>

Player::Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& room)
    : pos(point), screen(room){
    for (int i = 0; i < NUM_KEYS; ++i) {
        keys[i] = the_keys[i];
    }
}


Player& Player::operator=(const Player& other) {
    if (this != &other) {
        pos = other.pos;
        for (int i = 0; i < NUM_KEYS; ++i) {
            keys[i] = other.keys[i];
        }
        screen = other.screen;
        inventory = other.inventory;
        visible = other.visible;
    }
    return *this;
}


void Player::draw() {
	if (!visible) return;
    pos.draw();
}


void Player::pickUp() {
	if (!visible) return;

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

    // Redraw the room first, then paint torch overlay (if any).
    screen.printRoom();

    // If the picked-up item is a Torch, activate its effect immediately and update buffer
    if (Torch* t = dynamic_cast<Torch*>(inventory)) {
        t->torchEffect(screen, pos);
    }
}


CollectableItems* Player::dispose() {
	if (!visible) return nullptr;

    if (!inventory)
        return nullptr;

    auto res = pos.PlaceToDrop(screen, 1);
    bool found = res.first;
    if (!found)
        return nullptr;

    Point dropPos = res.second;

    // copy appearance (glyph + color) from the inventory into dropPos
    // this uses Point::operator=(const Item&), which preserves dropPos coordinates
    dropPos = *inventory;

    inventory->setPos(dropPos);

    // Let the item react to being dropped
    inventory->onDrop(*this, screen);

    // If it's a bomb, arm it (non-blocking) and transfer ownership to the screen
    Bomb* bomb = dynamic_cast<Bomb*>(inventory);
    if (bomb) {
        // transfer ownership to screen and arm fuse
        screen.addItem(bomb);
        constexpr int fuseTicks = 5; // explodes after 5 game-loop ticks
        bomb->arm(fuseTicks);

        screen.printRoom();
        inventory = nullptr;
        return nullptr;
    }

    // If it's a torch, transfer it to the screen and make it a persistent light source
    Torch* torch = dynamic_cast<Torch*>(inventory);
    if (torch) {
        screen.addItem(torch);
        // ensure the torch lights its area centered on its drop position
        torch->paintLight(screen, dropPos, 4);

        screen.printRoom();
        inventory = nullptr;
        return nullptr;
    }

    // Non-bomb: normal drop behavior -> add to screen as live item
    screen.addItem(inventory);
    screen.printRoom();
    CollectableItems* tmp = inventory;
    inventory = nullptr;
    return tmp;
}

CollectableItems* Player::takeInventory() {
    CollectableItems* tmp = inventory;
    inventory = nullptr;
    return tmp;
}

void Player::setVisible(bool v) {
	if (visible == v) return;

	if (!v) {
		// erase current glyph from console before hiding
		pos.draw(' ');
	} else {
		// if becoming visible again, draw at current pos
		pos.draw();
	}
	visible = v;
}

void Player::move() {
	if (!visible) return;

    // compute next
    Point next = pos;
    next.move();

    // block on walls/doors/items (like wall collision)
    if (screen.isWall(next) || screen.isItem(next)) {
        // redraw current position and don't move
        pos.draw();
        return;
    }

    // if carrying a torch, update light incrementally: compute diff between pos and next
    Torch* torch = nullptr;
    if (inventory) torch = dynamic_cast<Torch*>(inventory);
    if (torch) {
        torch->paintLightDiff(screen, pos, next);
    }

    // erase current and move
    pos.draw(' ');
    pos = next;
    pos.draw();
}

void Player::handleKeyPressed(char key_pressed) {
	if (!visible) return;

    size_t index = 0;
    for (char k : keys) {
        if (std::tolower(k) == std::tolower(key_pressed)) {
            if (k == 'o' || k == 'e') {
                if (inventory == nullptr) {
                    pickUp();
					index = static_cast<size_t>(Direction::STAY);           //stay in place after pickup
                    pos.setDirection(static_cast<Direction>(index));
                }
                else {
                    dispose();
				
                }
            }
            else {
                pos.setDirection(static_cast<Direction>(index));
                return;
            }
        }
        ++index;
    }
}
