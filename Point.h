#pragma once

#include "Console.h"
#include "Direction.h"
#include <utility>

class Screen; // Forward declaration
class Item;   // forward declare Item for operator=

class Point {
    int x = 1, y = 1;
    int diff_x = 1, diff_y = 0;
    char ch = '*';
    Color color = Color::White;

public:
    Point() {}
	Point(int x1, int y1) : x(x1), y(y1) {}

    Point(int x1, int y1, int diffx, int diffy, char c)
        : Point(x1, y1, diffx, diffy, c, Color::White) {}

    Point(int x1, int y1, char c, Color myColor)
		: x(x1), y(y1), ch(c), color(myColor) {
	}

    Point(int x1, int y1, int diffx, int diffy, char c, Color myColor)
        : x(x1), y(y1), diff_x(diffx), diff_y(diffy), ch(c), color(myColor) {}

    Point(const Point& p)
		: x(p.x), y(p.y), diff_x(p.diff_x), diff_y(p.diff_y), ch(p.ch), color(p.color) {
	}

    void draw() const {
        draw(ch);
    }

    void draw(char c) const;
    void move();
    void setDirection(Direction dir);

    int getX() const { return x; }
    int getY() const { return y; }
	char getCh() const { return ch; }
    Color getColor() const { return color; }

    void setCh(char c) { ch = c; }
    void setColor(Color c) { color = c; }

    // Assign appearance from an Item (copies glyph + color only — preserves x/y)
    Point& operator=(const Item& item);
    Point& operator=(const Point& other);
    Point& operator=(const Item* item) { if (item) return operator=(*item); return *this; }

    std::pair<bool, Point> ItemInRadios(Screen& screen, int radius) const;
    std::pair<bool, Point> PlaceToDrop(Screen& screen, int radius) const;

};
