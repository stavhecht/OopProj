#pragma once

#include "Point.h"
#include "CollectableItems.h"
#include "Screen.h"

class Player {
	static constexpr int NUM_KEYS = 6;
	Point pos;
	char keys[NUM_KEYS];
	Screen& screen;
	CollectableItems* inventory = nullptr;
public:
	// ctor gets start point and an array of 5 keys
	Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& room);

	// Existing parameterized constructor
	void draw();
	void move();
	void pickUp();
	void dispose();

	// Return current inventory pointer (may be nullptr)
	CollectableItems* getInventory() const { return inventory; }

	// Take (remove) the inventory from the player and return it (player no longer owns it).
	// Caller becomes responsible for the returned pointer. This is used when a door consumes a key.
	CollectableItems* takeInventory();

	void handleKeyPressed(char key);

	Point getPos() const { return pos; }
	void setPos(const Point& p) { pos = p; }

	Screen& getScreen() { return screen; }
	const Screen& getScreen() const { return screen; }
};
