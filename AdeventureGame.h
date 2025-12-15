#pragma once

#include "Player.h"
#include "Screen.h"

class AdeventureGame {
    enum { ESC = 27 };
    Screen screen;
    Player players[2];

private:
    // refactored helpers
    bool waitForMenuSelection(bool &exitApp);
    void startNewGame();
    void loadRoom(int currentRoom, int &movedToNextRoom);
    void processPlayersMovement(int &currentRoom, bool &changeRoom, int &movedToNextRoom, bool &running);
    void processSteppedOnInteractions(bool &changeRoom);
    void handleInput(bool &running, bool &changeRoom);

public:
    AdeventureGame();
    void init();
    void run();
};
