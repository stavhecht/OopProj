#include "Torch.h"
#include "Screen.h"
#include "Player.h"
#include "Console.h"
#include <iostream>


void Torch::torchEffect(Screen& screen, const Point& center) {
    // keep backward compatible: paint full area and update color buffer
    paintLight(screen, center, 4);
}

std::vector<Point> Torch::litCells(const Point& center, int radius) const {
    std::vector<Point> out;
    int cx = center.getX();
    int cy = center.getY();
    int minX = max(0, cx - radius);
    int maxXc = min(Screen::MAX_X - 1, cx + radius);
    int minY = max(0, cy - radius);
    int maxYc = min(Screen::MAX_Y - 1, cy + radius);

    for (int y = minY; y <= maxYc; ++y) {
        for (int x = minX; x <= maxXc; ++x) {
            int dx = x - cx;
            int dy = y - cy;
            if (dx * dx + dy * dy <= radius * radius) {
                out.emplace_back(x, y);
            }
        }
    }
    return out;
}

void Torch::paintLight(Screen& screen, const Point& center, int radius) const {
    auto cells = litCells(center, radius);
    // update per-cell color buffer and print
    for (const Point& p : cells) {
        screen.setCellColor(p, Color::Yellow);
        gotoxy(p.getX(), p.getY());
        set_color(Color::Yellow);
        std::cout << screen.getCharAtcurrentRoom(p);
        reset_color();
    }

    // redraw items on top
    for (const Point& p : cells) {
        Item* it = screen.peekItemAt(p);
        if (it) it->draw();
    }
}

void Torch::paintLightDiff(Screen& screen, const Point& oldCenter, const Point& newCenter) const {
    int radius = 4;
    auto prev = litCells(oldCenter, radius);
    auto next = litCells(newCenter, radius);
	int currentRoomIndex = screen.getCurrentRoomIndex();
    Color roomOrigColor = screen.getRoomDefaultColor(currentRoomIndex);

    // For each cell in prev that is not in next -> restore (reset color to default)
    for (const Point& p : prev) {
        bool stillLit = false;
        for (const Point& q : next) {
            if (p == q) { stillLit = true; break; }
        }
        if (!stillLit) {
            // restore underlying character color in buffer (default LightYellow)
            screen.setCellColor(p, roomOrigColor);
            Item* it = screen.peekItemAt(p);
            if (it) {
                it->draw(); // draw item which will use its own color
            } else {
                gotoxy(p.getX(), p.getY());
                set_color(roomOrigColor);
                std::cout << screen.getCharAtcurrentRoom(p);
                reset_color();
            }
        }
    }

    // For each cell in next that is not in prev -> paint yellow
    for (const Point& p : next) {
        bool wasLit = false;
        for (const Point& q : prev) {
            if (p == q) { wasLit = true; break; }
        }
        if (!wasLit) {
            screen.setCellColor(p, Color::Yellow);
            gotoxy(p.getX(), p.getY());
            set_color(Color::Yellow);
            std::cout << screen.getCharAtcurrentRoom(p);
            reset_color();

            // If there's an item on top, redraw it so it's visible
            Item* it = screen.peekItemAt(p);
            if (it) it->draw();
        }
    }
}
