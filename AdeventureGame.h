#pragma once

#include "Player.h"
#include "Screen.h"

class AdeventureGame {
    enum { ESC = 27 };
    Screen screen;
    Player players[2];
	

public:
    AdeventureGame();
    void init();
    void run();
};
