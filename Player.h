#pragma once

#include "Point.h"
#include "Item.h"
#include "Screen.h"

class Player {
	static constexpr int NUM_KEYS = 6;
	Point pos;
	char keys[NUM_KEYS];
	Screen& screen;
	Item* inventory = nullptr;
public:
	// ctor gets start point and an array of 5 keys
	Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& room);

	// Existing parameterized constructor
	void draw();
	void move();
	void pickUp();
	void dispose();
	Item* getInventory() const { return inventory; }
	void handleKeyPressed(char key);

	Point getPos() const { return pos; }
	void setPos(const Point& p) { pos = p; }

	Screen& getScreen() { return screen; }
	const Screen& getScreen() const { return screen; }
};
