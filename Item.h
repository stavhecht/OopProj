#pragma once
#include "Console.h"    
#include "Point.h"
#include <iostream>

class Item {
	Point pos;
	char ch;
	bool isPicked = false;
	Color color;

public:
	// Single constructor with defaults for color and picked state.
	Item(const Point& p, char c, Color col = Color::White, bool picked = false)
		: pos(p), ch(c), color(col), isPicked(picked) {}

	Point getPos() const { return pos; }

	char getCh() const { return ch; }

	bool getIsPicked() const { return isPicked; }

	// Set picked state according to argument
	void pickUp(bool picked) { isPicked = picked; }

	// Draw the item using its own color at its position
	void draw() {
		if (!isPicked) {
			gotoxy(pos.getX(), pos.getY());
			set_color(color);
			std::cout << ch << std::flush;
			reset_color();
		}
	}

	Item& drop() {
		isPicked = false;
		return *this;
	}
};

