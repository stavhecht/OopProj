#pragma once

#include "Player.h"
#include "Screen.h"

class AdeventureGame {
    enum { ESC = 27 };
    Player players[2];
    Screen screen;  

public:
    AdeventureGame();
    void init();
    void run();
};
