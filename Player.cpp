#include "Player.h"
#include "Bomb.h"
#include "Torch.h"
#include <cctype>

Player::Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& room)
    : pos(point), screen(room) {
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

    // get item from screen (removes it from screen.items)
    CollectableItems* item = dynamic_cast<CollectableItems*>(screen.getItem(itemPos));
    if (!item)
        return;

    inventory = item;

    // If the picked up item is a Torch, move the light from the ground to the player
    if (Torch* t = dynamic_cast<Torch*>(inventory)) {
        // restore cells previously lit by the torch on ground and paint around player
        t->paintLightDiff(screen, itemPos, pos);
    }

    // remove from room (getItem already removed, this is defensive)
    screen.changePixelInRoom(itemPos, ' ');

    // item reaction
    inventory->onPickUp(*this, screen);

    // Redraw the room
    screen.printRoom();

    // no extra torchEffect call — paintLightDiff already painted player's light
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

    // set drop position from inventory (uses operator Point conversion)
    dropPos = *inventory;
    inventory->setPos(dropPos);

	// item reaction
    inventory->onDrop(*this, screen);

    // If it's a bomb, arm it and transfer ownership to the screen
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
        // move light from player's position to the dropped position (restores old cells)
        torch->paintLightDiff(screen, pos, dropPos);

        // now transfer ownership to screen as a live item
        screen.addItem(torch);

        // final redraw
        screen.printRoom();
        inventory = nullptr;
        return nullptr;
    }

    // Non bomb, normal drop behavior add to screen as live item
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
		// erase current player sign from console before hiding
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
    if (!canMoveTo(next)) {
        // redraw current position and don't move
        pos.draw();
        return;
    }

    // if carrying a torch, update light incrementally: compute diff between pos and next
    updateTorchOnMove(next);

    // erase current and move
    pos.draw(' ');
    pos = next;
    pos.draw();
}

void Player::handleKeyPressed(char key_pressed) {
	if (!visible) return;

    size_t index = findKeyIndex(key_pressed);
    if (index < NUM_KEYS) {
        // pick/dispose keys handled specially
        char k = keys[index];
        if (k == 'o' || k == 'e') {
            handlePickupOrDispose(index);
        } else {
            setDirectionByIndex(index);
        }
    }
}


// --- private helpers ---

size_t Player::findKeyIndex(char key_pressed) const {
    size_t index = 0;
    for (char k : keys) {
        if (std::tolower(static_cast<unsigned char>(k)) == std::tolower(static_cast<unsigned char>(key_pressed))) {
            return index;
        }
        index++;
    }
    return NUM_KEYS;
}

void Player::handlePickupOrDispose(size_t /*index*/) {
    if (inventory == nullptr) {
        pickUp();
        // stay in place after pickup
        pos.setDirection(Direction::STAY);
    }
    else {
        dispose();
    }
}

void Player::setDirectionByIndex(size_t index) {
    if (index <= static_cast<size_t>(Direction::STAY)) {
        pos.setDirection(static_cast<Direction>(index));
    }
}

bool Player::canMoveTo(const Point& next) const {
    // block on walls/doors/items (like wall collision)
    if (screen.isWall(next) || screen.isItem(next))
        return false;
    return true;
}

void Player::updateTorchOnMove(const Point& next) {
    Torch* torch = nullptr;
    if (inventory) torch = dynamic_cast<Torch*>(inventory);
    if (torch) {
        torch->paintLightDiff(screen, pos, next);
    }
}
