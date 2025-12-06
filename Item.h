#pragma once
#include "Console.h"    
#include "Point.h"
#include <iostream>

class Player;
class Screen;

class Item {
protected:
    Point pos;
    char  ch;
	Color color = Color::White;

public:
    Item(const Point& p, char c, Color col) : pos(p), ch(c), color(col) {}
    virtual ~Item() = default;

    Point getPos() const { return pos; }
    void  setPos(const Point& p) { pos = p; }
    char  getCh() const { return ch; }

    // Called when something should happen:
    virtual void onPickUp(Player& player, Screen& screen) {}
    virtual void onDrop(Player& player, Screen& screen) {}
    virtual void onStep(Player& player, Screen& screen) {}
};


