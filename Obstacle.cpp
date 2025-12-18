#include "Obstacle.h"
#include "Screen.h"
#include "Player.h"
#include <queue>
#include <algorithm>

// Local helper to produce a Point stepped by one cell in dir
static Point stepPoint(const Point& p, Direction dir) {
    switch (dir) {
    case Direction::UP:    return Point(p.getX(), p.getY() - 1, p.getCh(), p.getColor());
    case Direction::RIGHT: return Point(p.getX() + 1, p.getY(), p.getCh(), p.getColor());
    case Direction::DOWN:  return Point(p.getX(), p.getY() + 1, p.getCh(), p.getColor());
    case Direction::LEFT:  return Point(p.getX() - 1, p.getY(), p.getCh(), p.getColor());
    default: return p;
    }
}

std::vector<Obstacle*> Obstacle::collectGroup(Screen& screen, const Point& start) {
    std::vector<Obstacle*> result;
    // Validate start is an Obstacle
    Item* rootIt = screen.peekItemAt(start);
    if (!rootIt) return result;
    Obstacle* root = dynamic_cast<Obstacle*>(rootIt);
    if (!root) return result;

    std::queue<Point> q;
    std::vector<Point> visited;

    q.push(start);
    visited.push_back(start);

    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { -1, 0, 1, 0 };

    while (!q.empty()) {
        Point p = q.front(); q.pop();
        Item* it = screen.peekItemAt(p);
        if (!it) continue;
        Obstacle* o = dynamic_cast<Obstacle*>(it);
        if (!o) continue;
        result.push_back(o);

        for (int i = 0; i < 4; ++i) {
            Point np(p.getX() + dx[i], p.getY() + dy[i]);
            bool seen = false;
            for (const Point& v : visited) {
                if (v == np) { seen = true; break; }
            }
            if (seen) continue;
            Item* nit = screen.peekItemAt(np);
            if (!nit) continue;
            Obstacle* no = dynamic_cast<Obstacle*>(nit);
            if (!no) continue;
            visited.push_back(np);
            q.push(np);
        }
    }

    return result;
}

bool Obstacle::canPushGroup(Screen& screen, const Point& start, Direction dir, int force) {
    auto group = collectGroup(screen, start);
    size_t size = group.size();
    if (size == 0) return false;
    if (static_cast<int>(size) > force) return false;

    // Ensure destinations (each member moved by dir) are not walls and do not contain non-group items.
    for (Obstacle* o : group) {
        Point dest = stepPoint(o->getPos(), dir);
        if (screen.isWall(dest)) return false;
        if (screen.isItem(dest)) {
            Item* it = screen.peekItemAt(dest);
            Obstacle* other = dynamic_cast<Obstacle*>(it);
            if (!other) return false;
            // if other is part of same group, ok (we will move all)
            bool found = false;
            for (Obstacle* g : group) {
                if (g == other) { found = true; break; }
            }
            if (!found) return false;
        }
    }
    return true;
}

bool Obstacle::pushGroup(Screen& screen, const Point& start, Direction dir) {
    auto group = collectGroup(screen, start);
    if (group.empty()) return false;

    // Order movement to avoid overwriting: for RIGHT/DOWN move far-first, for LEFT/UP move near-first.
    std::sort(group.begin(), group.end(), [dir](Obstacle* a, Obstacle* b) {
        if (dir == Direction::RIGHT) return a->getPos().getX() > b->getPos().getX();
        if (dir == Direction::LEFT) return a->getPos().getX() < b->getPos().getX();
        if (dir == Direction::DOWN) return a->getPos().getY() > b->getPos().getY();
        if (dir == Direction::UP) return a->getPos().getY() < b->getPos().getY();
        return false;
    });

    for (Obstacle* o : group) {
        Point p = o->getPos();
        Point np = stepPoint(p, dir);
        o->setPos(np);
    }

    return true;
}

void Obstacle::onStep(Player& /*player*/, Screen& /*screen*/) {
    // Pushing logic was previously implemented here and duplicated Player::move.
    // For a clearer single-responsibility design the push handling is centralized in Player::move:
    // - Player decides to move/push (has context about intended direction and cooperating players),
    // - Player invokes Obstacle::canPushGroup / Obstacle::pushGroup as helpers.
    //
    // Keep this method present for backwards compatibility but do not perform pushes here.
    return;
}
