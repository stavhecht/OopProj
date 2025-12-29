#pragma once
#include "SteppedOnItems.h"
#include "Screen.h"


class Switcher : public SteppedOnItems {
	bool onOff = false;
	int groupId = -1; // group = room number; -1 = ungrouped

public:
	// group: room number this switch belongs to (or -1)
	Switcher(const Point& p, char c = '/', Color col = Color::Red, int group = -1)
		: SteppedOnItems(p, c, col), groupId(group) {}

	Switcher(const Switcher&) = default;
	Switcher& operator=(const Switcher&) = default;

	bool IsOn() const { return onOff; }
	int GetGroup() const { return groupId; }

	void onStep(Player& player, Screen& screen) override;
	virtual Item* clone() const override { return new Switcher(*this); }

};

