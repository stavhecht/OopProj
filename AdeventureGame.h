#pragma once

#include "Player.h"
#include "Screen.h"
#include <vector>

class AdeventureGame {
    enum { ESC = 27 };
    Screen screen;
    Player players[2];

private:
    // refactored helpers
    bool waitForMenuSelection(bool &exitApp);
    void startNewGame();
    // playersMoved: vector sized to number of players; true = that player moved to next room
    void loadRoom(int currentRoom, const std::vector<bool>& playersMoved);
    void processPlayersMovement(int &currentRoom, bool &changeRoom, std::vector<bool> &playersMoved, bool &running);
    void processSteppedOnInteractions(bool &changeRoom);
    void handleInput(bool &running, bool &changeRoom);

public:
    AdeventureGame();
    void init();
    void run();
};
