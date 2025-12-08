#pragma once
#include "CollectableItems.h"


class Player;
class Screen;

class Torch : public CollectableItems {
public:
    Torch(const Point& p, char c = '!', Color col = Color::Yellow) : CollectableItems(p, c, col){}

    Torch(const Torch&) = default;
    Torch& operator=(const Torch&) = default;

    // Reveal around a center on the screen (only declaration here -> forward decl is enough)
    void torchEffect(Screen& screen, const Point& center, int radius = 3);

    virtual Item* clone() const override { return new Torch(*this); }
};

