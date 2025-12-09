#pragma once
#include "SteppedOnItems.h"
#include "Player.h"
#include "Screen.h"


class Riddle: public SteppedOnItems{
	bool answered = false;

	public:
	Riddle(const Point& p, char c = '?', Color col = Color::LightAqua) : SteppedOnItems(p, c, col) {}

	Riddle(const Riddle&) = default;
	Riddle& operator=(const Riddle&) = default;
	bool isAnswered() const { return answered; }

	void onStep(Player& player, Screen& screen) override;

	virtual Item* clone() const override { return new Riddle(*this); }
};

