#include "Torch.h"
#include "Screen.h"
#include "Player.h"
#

void Torch::torchEffect(Screen& screen, const Point& center, int radius) {
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
            if (dx*dx + dy*dy <= radius*radius) {
                Point p(x, y);
                screen.changePixelInCurrBoard(p, screen.getCharAtcurrentRoom(p));
            }
        }
    }
}
