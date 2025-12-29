#include "Point.h"
#include "Screen.h"
#include "Item.h" 
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

// Copy only appearance (pos + sign + color) from Item into this Point.
Point& Point::operator=(const Item& item) {
    Point ip = item.getPos(); 
    ch = ip.getCh();
    color = ip.getColor();
    return *this;
}

pair<bool, Point> Point::ItemInRadios(Screen& screen, int radius) const {
	// Search a square of side (2*radius+1) around this point, skipping own tile.
	// Return position of first CollectableItems found.
	for (int dy = -radius; dy <= radius; ++dy) {
		for (int dx = -radius; dx <= radius; ++dx) {
			if (dx == 0 && dy == 0)
				continue; // skip own position
			int newX = x + dx;
			int newY = y + dy;

            // bounds-check: avoid out-of-range accesses on the Screen buffers
            if (newX < 0 || newX >= Screen::MAX_X || newY < 0 || newY >= Screen::MAX_Y)
                continue;

			Point targetPoint(newX, newY);
			Item* it = screen.peekItemAt(targetPoint);
			if (!it)
				continue;

            // Only consider collectable items for this query
            CollectableItems* ci = dynamic_cast<CollectableItems*>(it);
            if (ci) {
                return make_pair(true, targetPoint);
            }
		}
	}
	return make_pair(false, Point(-1, -1));
}

pair<bool, Point> Point::SteppedOnAdjacent(Screen& screen) const {
    // Check four orthogonal neighbours (no diagonals) for SteppedOnItems.
    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { -1, 0, 1, 0 };

    for (int i = 0; i < 4; i++) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (nx < 0 || nx >= Screen::MAX_X || ny < 0 || ny >= Screen::MAX_Y)
            continue;

        Point target(nx, ny);
        Item* it = screen.peekItemAt(target);
        if (!it)
            continue;

        SteppedOnItems* si = dynamic_cast<SteppedOnItems*>(it);
        if (si) {
            return make_pair(true, target);
        }
    }

    return make_pair(false, Point(-1, -1));
}

pair<bool, Point> Point::PlaceToDrop(Screen& screen, int radius) const {

	const int orth_dx[4] = { 0, -1, 0, 1 };
	const int orth_dy[4] = { 1, 0, -1, 0 };

	for (int i = 0; i < 4; i++) {
		int nx = x + orth_dx[i];
		int ny = y + orth_dy[i];

		if (nx < 0 || nx >= Screen::MAX_X || ny < 0 || ny >= Screen::MAX_Y)
			continue;
		Point targetPoint(nx, ny);
		if (screen.getCharAtcurrentRoom(targetPoint) == ' ') {
			return make_pair(true, targetPoint);
		}
	}
	
	const int diag_dx[4] = { -1, 1, 1, -1 };
	const int diag_dy[4] = { -1, -1, 1, 1 };

	for (int i = 0; i < 4; i++) {
		int nx = x + diag_dx[i];
		int ny = y + diag_dy[i];

		if (nx < 0 || nx >= Screen::MAX_X || ny < 0 || ny >= Screen::MAX_Y)
			continue;
		Point targetPoint(nx, ny);
		if (screen.getCharAtcurrentRoom(targetPoint) == ' ') {
			return make_pair(true, targetPoint);
		}
	}
	
	return make_pair(false, Point(-1, -1));
}

