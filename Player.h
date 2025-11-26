#pragma once

#include "Point.h"
#include "Screen.h"

class Player {
	static constexpr int NUM_KEYS = 5;
	Point pos;
	char keys[NUM_KEYS];
	Screen& screen;
public:
	// ctor gets start point and an array of 5 keys
	Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& theScreen);

	// Existing parameterized constructor
	void draw();
	void move();
	void handleKeyPressed(char key);
};
