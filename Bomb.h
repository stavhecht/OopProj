#pragma once
#include "CollectableItems.h"

//class Player;
//class Screen;

class Bomb : public CollectableItems {
	public:
	Bomb(const Point& p, char c = '@', Color col = Color::Red) : CollectableItems(p, c, col) {}
	Bomb(const Bomb&) = default;
	Bomb& operator=(const Bomb&) = default;

	// Trigger bomb effect on the screen (only declaration here -> forward decl is enough)
	void triggerBomb(Screen& screen, const Point& center);

	virtual Item* clone() const override { return new Bomb(*this); }
};

