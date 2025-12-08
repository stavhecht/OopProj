#pragma once
#include "Console.h"    
#include "Point.h"
#include <iostream>

class Player;
class Screen;

class Item {
protected:
    Point pos;

public:
    Item(const Point& p, char c, Color col) : pos(p.getX(), p.getY(), c, col) {}
    virtual ~Item() = default;

    Point getPos() const { return pos; }
    void  setPos(const Point& p) { pos = p; }
    char  getCh() const { return pos.getCh(); }
    void draw() const { pos.draw(); }

    // Polymorphic clone used by Screen copy/assign
    virtual Item* clone() const { return new Item(*this); }

    // Called when something should happen:
    virtual void onPickUp(Player& player, Screen& screen) {}
    virtual void onDrop(Player& player, Screen& screen) {}
    virtual void onStep(Player& player, Screen& screen) {}
};


