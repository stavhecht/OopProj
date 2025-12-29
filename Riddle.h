#pragma once
#include "SteppedOnItems.h"
#include "Screen.h"
#include "Player.h"

class Riddle: public SteppedOnItems{
	bool answered = false;
	string q = "";
	string a = "";

	public:
	Riddle(const Point& p, char c = '?', Color col = Color::LightAqua) : SteppedOnItems(p, c, col) {}
	// New ctor that stores question/answer for this Riddle instance
	Riddle(const Point& p, char c, Color col, const string& question, const string& answer)
		: SteppedOnItems(p, c, col), q(question), a(answer) {}

	Riddle(const Riddle&) = default;
	Riddle& operator=(const Riddle&) = default;
	bool isAnswered() const { return answered; }

	void onStep(Player& player, Screen& screen) override;

	virtual Item* clone() const override { return new Riddle(*this); }

	// optional accessors (not required, but useful)
	const string& getQuestion() const { return q; }
	const string& getAnswer() const { return a; }
};

