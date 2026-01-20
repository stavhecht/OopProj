#pragma once

#include "Player.h"
#include "console.h"
#include "Screen.h"
#include "ReplayIO.h"
#include <vector>

class AdeventureGame {
    enum { ESC = 27 };
    Screen screen;
    Player players[2];
    // Replay / mode state
    ReplayIO* activeIO = nullptr;
    bool loadMode = false;
    bool silentMode = false;
    int tickCounter = 0;

private:
    // refactored helpers
    bool waitForMenuSelection(bool &exitApp);
    void startNewGame();
    // playersMoved: vector sized to number of players; true = that player moved to next room
    void loadRoom(int currentRoom, const vector<bool>& playersMoved);
    void processPlayersMovement(int &currentRoom, bool &changeRoom, vector<bool> &playersMoved, bool &running);
    void processSteppedOnInteractions(bool &changeRoom);
    void handleInput(bool &running, bool &changeRoom);
    void handleInputWithRecord(ReplayIO& io, int tick, bool& running, bool& changeRoom);
    static Point GetStartPosForRoom(int room, int playerIndex);
    pair<bool, int>loadFiles(vector<vector<string>>& mapData);

public:
    AdeventureGame();
    bool init();
    void run(bool isLoad, bool isSave, bool isSilent);
};
