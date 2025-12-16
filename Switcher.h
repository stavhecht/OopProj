#pragma once
#include "SteppedOnItems.h"
#include "Screen.h"


class Switcher : public SteppedOnItems {
	bool onOff = false;

public:
	Switcher(const Point& p, char c = '/', Color col = Color::Red) : SteppedOnItems(p, c, col) {}

	Switcher(const Switcher&) = default;
	Switcher& operator=(const Switcher&) = default;
	bool IsOn() const { return onOff; }

	void onStep(Player& player, Screen& screen) override;
	virtual Item* clone() const override { return new Switcher(*this); }

};

