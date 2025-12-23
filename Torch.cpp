#include "Torch.h"
#include "Screen.h"
#include "Player.h"
#include "Console.h"
#include <iostream>


void Torch::torchEffect(Screen& screen, const Point& center) {
    // keep backward compatible: paint full area and update color buffer
    paintLight(screen, center, 4);
}

vector<Point> Torch::litCells(const Point& center, int radius) const {
    vector<Point> out;
    int cx = center.getX();
    int cy = center.getY();
    int minX = max(0, cx - radius);
    int maxXc = min(Screen::MAX_X - 1, cx + radius);
    int minY = max(0, cy - radius);
    int maxYc = min(Screen::MAX_Y - 1, cy + radius);

    for (int y = minY; y <= maxYc; y++) {
        // do not light any row with y > 22 (display row)
        if (y > 22) continue;

        for (int x = minX; x <= maxXc; x++) {
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

    // choose paint color depending on whether the room uses coloring;
    // when colors are OFF we paint torch light in white so it remains visible.
    int currentRoomIndex = screen.getCurrentRoomIndex();
    Color litColor = screen.isRoomUseColor(currentRoomIndex) ? Color::Yellow : Color::White;

    // update per-cell color buffer and print
    for (const Point& p : cells) {
        screen.setCellColor(p, litColor);
        gotoxy(p.getX(), p.getY());
        set_color(litColor);
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

    // determine lit color for this room (white if coloring is off)
    Color litColor = screen.isRoomUseColor(currentRoomIndex) ? Color::Yellow : Color::White;

    // For each cell in prev that is not in next, restore (reset color to default)
    for (const Point& p : prev) {
        bool stillLit = false;
        for (const Point& q : next) {
            if (p == q) { stillLit = true; break; }
        }
        if (!stillLit) {
            // restore underlying character color in buffer
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

    // For each cell in next that is not in prev 
    for (const Point& p : next) {
        bool wasLit = false;
        for (const Point& q : prev) {
            if (p == q) { wasLit = true; break; }
        }
        if (!wasLit) {
            screen.setCellColor(p, litColor);
            gotoxy(p.getX(), p.getY());
            set_color(litColor);
            std::cout << screen.getCharAtcurrentRoom(p);
            reset_color();

            // If there's an item on top, redraw it so it's visible
            Item* it = screen.peekItemAt(p);
            if (it) it->draw();
        }
    }
}
