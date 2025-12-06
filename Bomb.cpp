#include "Bomb.h"
#include "Screen.h"
#include "Player.h"

void Bomb::triggerBomb(Screen& screen, const Point& center) {
	int radius = 3;
	for (int dy = -radius; dy < radius; dy++) {
		for(int dx = -radius; dx < radius; dx++) {
			int distSq = dx * dx + dy * dy;
			if (distSq <= radius * radius) {
				int x = center.getX() + dx;
				int y = center.getY() + dy;
				if (x >= 0 && x < Screen::MAX_X && y >= 0 && y < Screen::MAX_Y) {
					Point p(x, y);
					screen.changePixelInCurrBoard(p, ' ');
				}
			}
		}
	}
}