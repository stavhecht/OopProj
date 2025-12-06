#pragma once
#include "CollectableItems.h"

//class Player;
//class Screen;

class Bomb : public CollectableItems {
	public:
	Bomb(const Point& p, char c = '@', Color col = Color::Red) : CollectableItems(p, c, col) {}
	// Trigger bomb effect on the screen (only declaration here -> forward decl is enough)
	void triggerBomb(Screen& screen, const Point& center);
};

