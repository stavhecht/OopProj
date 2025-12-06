#pragma once
#include "SteppedOnItems.h"
#include "Player.h"
#include "Screen.h"


class Riddle: public SteppedOnItems{
	bool answered = false;

	public:
	Riddle(const Point& p, char c = '?', Color col = Color::LightAqua) : SteppedOnItems(p, c, col) {}

	void onStep(Player& player, Screen& screen) override;
};

