#include "Spring.h"
#include "Screen.h"
#include "Player.h"

// Helper to step point one cell in Direction
static Point stepPointDir(const Point& p, Direction dir) {
    switch (dir) {
    case Direction::UP:    return Point(p.getX(), p.getY() - 1);
    case Direction::RIGHT: return Point(p.getX() + 1, p.getY());
    case Direction::DOWN:  return Point(p.getX(), p.getY() + 1);
    case Direction::LEFT:  return Point(p.getX() - 1, p.getY());
    default: return p;
    }
}

void Spring::onStep(Player& player, Screen& screen) {
    // Called when player is adjacent (Point::SteppedOnAdjacent). Determine direction from player -> this spring cell.
    Point sprPos = this->getPos();
    Point plPos = player.getPos();

    int dx = sprPos.getX() - plPos.getX();
    int dy = sprPos.getY() - plPos.getY();

    Direction dir = Direction::STAY;
    if (dx == 1 && dy == 0) dir = Direction::RIGHT;
    else if (dx == -1 && dy == 0) dir = Direction::LEFT;
    else if (dx == 0 && dy == 1) dir = Direction::DOWN;
    else if (dx == 0 && dy == -1) dir = Direction::UP;
    else return; // not directly adjacent in orthogonal direction

    // Count consecutive spring chars from the spring cell towards the wall (including this cell)
    int count = 0;
    Point cur = sprPos;
    while (true) {
        Item* it = screen.peekItemAt(cur);
        Spring* s = dynamic_cast<Spring*>(it);
        if (!s) break;
        ++count;
        Point np = stepPointDir(cur, dir);
        if (screen.isWall(np)) break; // stop at wall
        cur = np;
    }

    if (count <= 0) return;

    // Release immediately: speed = count, duration = count*count
    int speed = count;
    int duration = speed * speed;
    player.applyLaunch(speed, dir, duration);
}
