#pragma once
#include "CollectableItems.h"
#include <vector>

class Player;
class Screen;

class Torch : public CollectableItems {
public:
    Torch(const Point& p, char c = '!', Color col = Color::Yellow) : CollectableItems(p, c, col){}

    Torch(const Torch&) = default;
    Torch& operator=(const Torch&) = default;

    // Reveal around a center on the screen 
    void torchEffect(Screen& screen, const Point& center);

    // Return list of points light for a given center (radius default 4)
    std::vector<Point> litCells(const Point& center, int radius = 4) const;

    // Paint entire light area for `center` (yellow) and redraw any items in the area
    void paintLight(Screen& screen, const Point& center, int radius = 4) const;

    // Paint difference between oldCenter and newCenter:
    // - cells newly lit -> paint yellow
    // - cells no longer lit -> restore underlying char or item
    void paintLightDiff(Screen& screen, const Point& oldCenter, const Point& newCenter) const;

    virtual Item* clone() const override { return new Torch(*this); }
};

