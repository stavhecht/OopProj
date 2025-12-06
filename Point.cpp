#include "Point.h"
#include "Screen.h"


void Point::draw(char c) {
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

std::pair<bool, Point> Point::ItemInRadios(Screen& screen, int radius) const {
	for (int dy = -radius; dy <= radius; ++dy) {
		for (int dx = -radius; dx <= radius; ++dx) {
			int newX = (x + dx + Screen::MAX_X) % (Screen::MAX_X+1);
			int newY = (y + dy + Screen::MAX_Y) % Screen::MAX_Y;
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
			int newX = (x + dx + Screen::MAX_X) % (Screen::MAX_X+1);
			int newY = (y + dy + Screen::MAX_Y) % Screen::MAX_Y;
			Point targetPoint(newX, newY);
			if (screen.getCharAtcurrentBoard(targetPoint) == ' ') {
				return std::make_pair(true, targetPoint);
			}
		}
	}
	return std::make_pair(false, Point(-1, -1));
}

