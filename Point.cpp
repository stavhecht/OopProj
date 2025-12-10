#include "Point.h"
#include "Screen.h"
#include "Item.h" // full Item definition needed for operator=
#include <iostream>

Point& Point::operator=(const Point& other) {
	if(this != &other) {
		x = other.x;
		y = other.y;
		diff_x = other.diff_x;
		diff_y = other.diff_y;
		ch = other.ch;
		color = other.color;
	}
	return *this;
}


void Point::draw(char c) const {
	gotoxy(x, y);
	set_color(color);
	std::cout << c << std::flush;
	reset_color();
}

void Point::move() {
	x = (x + diff_x + Screen::MAX_X) % Screen::MAX_X;
	y = (y + diff_y + Screen::MAX_Y) % Screen::MAX_Y;
}

void Point::setDirection(Direction dir) {
	switch (dir) {
	case Direction::UP:
		diff_x = 0;
		diff_y = -1;
		break;
	case Direction::RIGHT:
		diff_x = 1;
		diff_y = 0;
		break;
	case Direction::DOWN:
		diff_x = 0;
		diff_y = 1;
		break;
	case Direction::LEFT:
		diff_x = -1;
		diff_y = 0;
		break;
	case Direction::STAY:
		diff_x = 0;
		diff_y = 0;
		break;
	}
}

// Copy only appearance (glyph + color) from Item into this Point.
// Note: preserves this Point's x/y — intended for "apply item's look to a coordinate".
Point& Point::operator=(const Item& item) {
    Point ip = item.getPos(); // use item's internal point for glyph/color
    ch = ip.getCh();
    color = ip.getColor();
    return *this;
}

std::pair<bool, Point> Point::ItemInRadios(Screen& screen, int radius) const {
	for (int dy = -radius; dy <= radius; ++dy) {
		for (int dx = -radius; dx <= radius; ++dx) {
			int newX = x + dx;
			int newY = y + dy;

            // bounds-check: avoid out-of-range accesses on the Screen buffers
            if (newX < 0 || newX >= Screen::MAX_X || newY < 0 || newY >= Screen::MAX_Y)
                continue;

			Point targetPoint(newX, newY);
			if (screen.isItem(targetPoint)) {
				return std::make_pair(true, targetPoint);
			}
		}
	}
	return std::make_pair(false, Point(-1, -1));
}

std::pair<bool, Point> Point::PlaceToDrop(Screen& screen, int radius) const {
	for (int dy = -radius; dy <= radius; ++dy) {
		for (int dx = -radius; dx <= radius; ++dx) {
			if(dx == 0 && dy == 0)
				continue; // skip own position
			int newX = x + dx;
			int newY = y + dy;

            // bounds-check: ensure we don't query screen buffers with invalid indices
            if (newX < 0 || newX >= Screen::MAX_X || newY < 0 || newY >= Screen::MAX_Y)
                continue;

			Point targetPoint(newX, newY);
			if (screen.getCharAtcurrentRoom(targetPoint) == ' ') {
				return std::make_pair(true, targetPoint);
			}
		}
	}
	return std::make_pair(false, Point(-1, -1));
}

