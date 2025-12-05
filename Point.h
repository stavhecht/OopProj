#pragma once

#include "Console.h"
#include "Direction.h"
#include <utility>

class Screen; // Forward declaration

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

    Point(int x1, int y1, int diffx, int diffy, char c, Color myColor)
        : x(x1), y(y1), diff_x(diffx), diff_y(diffy), ch(c), color(myColor) {}

    Point(const Point& p)
		: x(p.x), y(p.y), diff_x(p.diff_x), diff_y(p.diff_y), ch(p.ch), color(p.color) {
	}

    void draw() {
        draw(ch);
    }

    void draw(char c);
    void move();
    void setDirection(Direction dir);

    int getX() const { return x; }
    int getY() const { return y; }

   
    std::pair<bool, Point> ItemInRadios(Screen& screen, int radius) const;
    std::pair<bool, Point> PlaceToDrop(Screen& screen, int radius) const;

};
