#include "AdeventureGame.h"
#include "Console.h"
#include <fstream>
#include <vector>
#include <string>
#include <map>



    // Return default start position for a given room/player
Point AdeventureGame::GetStartPosForRoom(int room, int playerIndex){
    if (room == 1) {
        return (playerIndex == 0) ? Point(10, 10, 0, 0, '$', Color::Red)
                                    : Point(15, 5, 0, 0, '&', Color::Blue);
    }
    else if (room == 2) {
        return (playerIndex == 0) ? Point(31, 3, 0, 0, '$', Color::Red)
                                    : Point(34, 3, 0, 0, '&', Color::Blue);
    }
    else if (room == 3) {
        return (playerIndex == 0) ? Point(2, 2, 0, 0, '$', Color::Red)
                                    : Point(2, 4, 0, 0, '&', Color::Blue);
    }

    // fallback
    return (playerIndex == 0) ? Point(1, 1, 0, 0, '$', Color::Red)
                                    : Point(2, 1, 0, 0, '&', Color::Blue);
    }


AdeventureGame::AdeventureGame()
    : screen(Screen())
    , players{ Player(Point(10, 10, 1, 0, '$', Color::Red), "wdxase", screen),
               Player(Point(15, 5, 0, 1, '&', Color::Blue), "ilmjko", screen) }
{}


pair<bool, int> AdeventureGame::loadFiles(vector<vector<string>>& mapData) {
    mapData.clear();
    const int ROOM_COUNT = 3;
    mapData.resize(ROOM_COUNT); // assuming 3 rooms

    for (int i = 0; i < ROOM_COUNT; i++) {
        string filename = "adv-world_0" + to_string(i + 1) + ".screen.txt";
        fstream file(filename);

        if (!file.is_open()) {
            return make_pair(false, i+1);
        }

        string line;
        while (getline(file, line)) {
            mapData[i].push_back(line);
        }

        file.close();
    }

    // If none of the files could be used, signal failure
    return make_pair(true, 0);
}

bool AdeventureGame::init()
{
    init_console();
    clrscr();
    hideCursor();
	auto isLoaded = loadFiles(screen.getGameRoomsData());
    // Try to load as many files as possible; if none usable, let caller decide to exit
    if (!isLoaded.first) {
        screen.setErrorBoard(isLoaded.second);
        screen.printBoard();
        while (true){
            if (check_kbhit()) {
                char c = static_cast<char>(get_single_char());
                if (c == ESC) {
                    break;
                }
            }
        }
		
		return true;
    }

    
    // room: <n>\n
    // q: <question>\n
    // a: <answer>\n
    // blank line separates entries.
    {
        map<int, pair<string, string>> qa;
        ifstream rf("riddles.txt");
        if (rf.is_open()) {
            string line;
            int room = -1;
            string q, a;
            auto trim = [](string s){
                size_t b = s.find_first_not_of(" \t");
                size_t e = s.find_last_not_of(" \t");
                if (b == string::npos) return string();
                string out = s.substr(b, e - b + 1);
                // strip trailing CR (Windows files)
                if (!out.empty() && out.back() == '\r') out.pop_back();
                return out;
            };
            auto commit = [&]() {
                if (room >= 1 && room <= 3 && !q.empty() && !a.empty()) {
                    qa[room] = make_pair(q, a);
                }
                room = -1; q.clear(); a.clear();
            };
            while (std::getline(rf, line)) {
                line = trim(line);
                if (line.empty()) { commit(); continue; }
                // lowercase copy for case-insensitive matching
                string lowerLine = line;
                for (char &c : lowerLine) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
                if (lowerLine.rfind("room:", 0) == 0) {
                    commit();
                    string num = trim(line.substr(5));
                    try { room = stoi(num); } catch (...) { room = -1; }
                } else if (lowerLine.rfind("room ", 0) == 0) {
                    commit();
                    string num = trim(line.substr(5));
                    try { room = std::stoi(num); } catch (...) { room = -1; }
                } else if (lowerLine.rfind("q:", 0) == 0) {
                    q = trim(line.substr(2));
                } else if (lowerLine.rfind("a:", 0) == 0) {
                    a = trim(line.substr(2));
                } else {
                    // if line is just digits, treat as room indicator
                    bool allDigits = !line.empty();
                    for (char c : line) if (!isdigit(static_cast<unsigned char>(c))) { allDigits = false; break; }
                    if (allDigits) {
                        commit();
                        try { room = std::stoi(line); } catch (...) { room = -1; }
                    }
                }
            }
            commit();
            rf.close();
            screen.setRiddlesQA(qa);
        }
    }
	return false;

}

bool AdeventureGame::waitForMenuSelection(bool& exitApp)
{
    screen.setMenu();
    screen.printBoard();

    bool startGame = false;

    while (!startGame && !exitApp) {
        if (check_kbhit()) {
            char key = static_cast<char>(get_single_char());

            if (key == '1') {
                startGame = true; // start the game loop
            }
            else if (key == '2') {
                // Toggle colors ON/OFF and redraw menu
                toggle_colors();
                screen.setMenu();
                screen.printBoard();
            }
            else if (key == '3') {
                screen.setScoreBoard();
                screen.printBoard();
                screen.gobacktoMenu();
            }
            else if (key == '8') {
                screen.setGuide();
                screen.printBoard();
                screen.gobacktoMenu();
            }
            else if (key == '9') {
                // exit application
                cleanup_console();
                clrscr();
                exitApp = true;
                break;
            }
        }

        sleep_ms(50);
    }

    return startGame;
}

void AdeventureGame::loadRoom(int currentRoom, const vector<bool>& playersMoved)
{
    screen.setRoom(currentRoom);

    // compute start positions for the room (only for players with lives)
    Point startPos[2];
    const int playerCount = static_cast<int>(sizeof(players) / sizeof(players[0]));

    if (currentRoom == 1 || currentRoom == 2 || currentRoom == 3) {
        for (int i = 0; i < playerCount; ++i) {
            if (players[i].getLifes() > 0) {
                startPos[i] = GetStartPosForRoom(currentRoom, i);
            }
        }
    }

    // helper lambdas
    auto clearInventory = [](Player& pl) {
        CollectableItems* inv = pl.takeInventory();
        if (inv) delete inv;
    };

    auto hidePlayer = [](Player& pl) {
        pl.setVisible(false);
        pl.setPos(Point(-1, -1));
    };

    // clear inventories for all players
    for (Player& p : players) {
        clearInventory(p);
    }

    // If playersMoved is empty or all false -> spawn all players.
    bool anyMoved = false;
    for (bool b : playersMoved) {
        if (b) { anyMoved = true; break; }
    }

    for (int i = 0; i < playerCount; ++i) {
        if (!anyMoved) {
            // initial spawn: enable players only if they have lives
            if (players[i].getLifes() > 0) {
                players[i] = startPos[i];
                players[i].setVisible(true);
            } else {
                hidePlayer(players[i]);
            }
        }
        else {
            if (i < static_cast<int>(playersMoved.size()) && playersMoved[i]) {
                // only spawn players that moved
                if (players[i].getLifes() > 0) {
                    players[i] = startPos[i];
                    players[i].setVisible(true);
                } else {
                    hidePlayer(players[i]);
                }
            }
            else {
                // hide players that didn't move
                hidePlayer(players[i]);
            }
        }
    }

    // Detect life drops and respawn affected players
    bool anyRespawned = false;
    for (int i = 0; i < playerCount; ++i) {
        if (players[i].hasDied()) {
            if (players[i].getLifes() > 0) {
                Point sp = GetStartPosForRoom(currentRoom, i);
                players[i] = sp;
                players[i].setVisible(true);
            } else {
                hidePlayer(players[i]);
            }

            // clear inventory (consistent with loadRoom behavior)
            clearInventory(players[i]);
            anyRespawned = true;
        }
    }

    // Always print the room once after load/respawn so the display is consistent.
    screen.printRoom();
    (void)anyRespawned; // keep variable if future logic needs it
}

void AdeventureGame::processPlayersMovement(int& currentRoom, bool& changeRoom, std::vector<bool>& playersMoved, bool& running)
{
    const int playerCount = static_cast<int>(sizeof(players) / sizeof(players[0]));

    for (int i = 0; i < playerCount; ++i) {
        Player& p = players[i];
        if (!p.isVisible())
            continue;

        // --- EARLY CHECK: if the player's next intended cell is an opened door -> treat as moving to next room.
        Point nextBeforeMove = p.getPos();
        nextBeforeMove.move(); // compute the next cell without calling private Player helpers
        bool nextIsOpenedDoor = false;

        // check live item door first
        Item* itNext = screen.peekItemAt(nextBeforeMove);
        if (itNext) {
            Door* dNext = dynamic_cast<Door*>(itNext);
            if (dNext && dNext->isOpen()) nextIsOpenedDoor = true;
        }

        // fallback: consider persisted opened doors (in case door item was removed)
        if (!nextIsOpenedDoor && screen.isDoorOpenedAt(nextBeforeMove)) {
            nextIsOpenedDoor = true;
        }

        if (nextIsOpenedDoor) {
            // mark player as moved to next room (hide & off-screen), preserve existing scoring behaviour
            p.setVisible(false);
            p.addScore(100);
            p.setPos(Point(-1, -1));

            if (i >= static_cast<int>(playersMoved.size()))
                playersMoved.resize(playerCount, false);
            playersMoved[i] = true;

            // Advance room when there are no visible players left.
            if (screen.getVisiblePlayerCount() == 0) {
                currentRoom++;
                for (int j = 0; j < playerCount; ++j) {
                    Player& pp = players[j];
                    if (!pp.isVisible()) continue;
                    pp.addScore(100); // bonus for completing room
                }
                changeRoom = true;
                break; // all remaining players moved to next room / none remain
            }

            // skip performing the normal move() for this player (we already handled transition)
            continue;
        }

        // If not an early transition, perform normal movement and then handle stepping onto opened door.
        p.move();

        Point playerPos = p.getPos();
        // if the player moved onto a previously-opened door tile, mark them moved and hide
        if (screen.isDoorOpenedAt(playerPos)) {
            p.setVisible(false);
            p.addScore(100);
            p.setPos(Point(-1, -1)); // move off-screen

            if (i >= static_cast<int>(playersMoved.size()))
                playersMoved.resize(playerCount, false);
            playersMoved[i] = true;

            // Advance room when there are no visible players left.
            if (screen.getVisiblePlayerCount() == 0) {
                currentRoom++;
                for (int j = 0; j < playerCount; j++) {
                    Player& p = players[j];
                    if (!p.isVisible()) continue;
                    p.addScore(100); // bonus for completing room
                }
                changeRoom = true;
                break; // all remaining players moved to next room / none remain
            }
        }
    }
}

void AdeventureGame::processSteppedOnInteractions(bool& changeRoom)
{
    const int dx[4] = { 0, 1, 0, -1 };
    const int dy[4] = { -1, 0, 1, 0 };

    const int playerCount = static_cast<int>(sizeof(players) / sizeof(players[0]));

    // persistent previous-adjacency per player so we only trigger onStep when adjacency changes
    static vector<vector<Point>> prevAdjacent;
    if (static_cast<int>(prevAdjacent.size()) != playerCount) {
        prevAdjacent.assign(playerCount, vector<Point>());
    }

    for (int pi = 0; pi < playerCount; pi++) {
        Player& p = players[pi];
        if (!p.isVisible())
            continue;

        // build current adjacency list (unique positions)
        vector<Point> curAdj;
        Point playerPos = p.getPos();
        for (int i = 0; i < 4; i++) {
            int nx = playerPos.getX() + dx[i];
            int ny = playerPos.getY() + dy[i];

            if (nx < 0 || nx >= Screen::MAX_X || ny < 0 || ny >= Screen::MAX_Y)
                continue;

            Point target(nx, ny);
            Item* adjItem = screen.peekItemAt(target);
            if (!adjItem) continue;

            SteppedOnItems* sItemAdj = dynamic_cast<SteppedOnItems*>(adjItem);
            if (!sItemAdj) continue;

            // avoid duplicates
            bool seen = false;
            for (const Point& pp : curAdj) {
                if (pp == target) { seen = true; break; }
            }
            if (!seen) curAdj.push_back(target);
        }

        // For each currently adjacent item that was NOT adjacent in previous tick -> trigger onStep
        for (const Point& pos : curAdj) {
            bool wasAdjacent = false;
            for (const Point& prevPos : prevAdjacent[pi]) {
                if (prevPos == pos) { wasAdjacent = true; break; }
            }
            if (!wasAdjacent) {
                Item* adjItem = screen.peekItemAt(pos);
                if (!adjItem) continue;
                SteppedOnItems* sItemAdj = dynamic_cast<SteppedOnItems*>(adjItem);
                if (!sItemAdj) continue;

                // Only trigger when adjacency has just appeared (player moved next to it)
                sItemAdj->onStep(p, screen);
            }
        }

        // Update prevAdjacent for this player
        prevAdjacent[pi].swap(curAdj);
    }
}

void AdeventureGame::handleInput(bool& running, bool& changeRoom)
{
    if (!check_kbhit())
        return;

    char key = static_cast<char>(get_single_char());
    if (key == ESC) {
        // Save current room state (chars + per-cell colors), then show paused board
        screen.saveStateForPause();
        screen.setGamePaused();
        screen.printBoard();

        // Wait for sub-key.
        bool requestMenu = false;
        bool requestUnpause = false;

        while (true) {
            char sub = static_cast<char>(get_single_char());
            if (sub == 'H' || sub == 'h') {
                requestMenu = true;
                break;
            }
            if (sub == ESC) {
                requestUnpause = true;
                break;
            }
            // ignore other keys - stay paused
        }

        if (requestMenu) {
            // Exit the game loop so outer menu is shown
            running = false;
            return;
        }

        if (requestUnpause) {
            // Restore prior room + colors and resume.
            screen.restoreStateFromPause();
            screen.printRoom();
            return;
        }
    }
    else {
        // Normal key handling for visible players only
        for (auto& p : players) {
            if (p.isVisible())
                p.handleKeyPressed(key);
        }
    }
}

void AdeventureGame::startNewGame()
{
    bool running = true;
    bool changeRoom = true;
    int currentRoom = 1;
    const int playerCount = static_cast<int>(sizeof(players) / sizeof(players[0]));
    vector<bool> playersMoved(playerCount, false);

    screen.registerPlayers(players, playerCount);

    while (running) {
        screen.updateBombs(); // update pending bombs

        // Detect which players lost a life this tick (consume the event via hasDied())
        vector<int> diedIndices;
        for (int i = 0; i < playerCount; i++) {
            if (players[i].hasDied()) {
                diedIndices.push_back(i);
            }
        }

        // If any player died -> respawn them without reloading the room (keep room state)
        if (!diedIndices.empty()) {
            for (int idx : diedIndices) {
                if (players[idx].getLifes() > 0) {
                    Point sp = GetStartPosForRoom(currentRoom, idx);
                    players[idx] = sp;
                    players[idx].setVisible(true);
                } else {
                    players[idx].setVisible(false);
                    players[idx].setPos(Point(-1, -1));
                }

                // clear inventory consistent with loadRoom semantics
                CollectableItems* inv = players[idx].takeInventory();
                if (inv) delete inv;
            }

            // update visuals once after all respawns
            screen.printRoom();
        }

        if (changeRoom) {
            if (currentRoom == 1 || currentRoom == 2 || currentRoom == 3) {
                loadRoom(currentRoom, playersMoved);
            }
            else if (currentRoom > 3) {
                // Game won!
                screen.setWin();
                screen.printBoard();
                running = false;
                while (true) {
                    if (check_kbhit()) {
                        char c = static_cast<char>(get_single_char());
                        if (c == ESC) {
                            break;
                        }
                    }
                }
                break;
            }
            changeRoom = false;
            // reset moved flags for next room
            fill(playersMoved.begin(), playersMoved.end(), false);
        }

        // Player movement and opened doors
        processPlayersMovement(currentRoom, changeRoom, playersMoved, running);
        if (changeRoom) {
            // reload next room on next iteration
            continue;
        }

        // CHECK FOR STEPPED-ON INTERACTIONS AFTER MOVEMENT
        processSteppedOnInteractions(changeRoom);
        if (changeRoom) {
            // next loop iteration will reload room
            continue;
        }

        // Only check for game-over no visible players
        if (screen.getVisiblePlayerCount() == 0) {
            screen.setLose();
            screen.printBoard();
            if (check_kbhit()) char c = get_single_char();
            while (true) {
                if (check_kbhit()) {
                    char c = static_cast<char>(get_single_char());
                    if (c == ESC) {
                        running = false;
                        break;
                    }
                }
            }
        }

        if (!running)
            break; // Break out of the game loop

        // Input handling (Pause menu, etc.)
        handleInput(running, changeRoom);

        sleep_ms(100);
    }

    // When startNewGame returns, control goes back to main menu loop.
}

void AdeventureGame::run() {
    bool exitApp = init();

    while (!exitApp) {
        bool startGame = waitForMenuSelection(exitApp);
        if (exitApp) {
            break;
        }

        if (startGame) {
            // Prepare players for a fresh game start without changing game-loop logic.
            // Clear any leftover inventory, reset lifes and hide them (loadRoom will position/ show as needed).
            for (Player& p : players) {
                CollectableItems* inv = p.takeInventory();
                if (inv) delete inv;
                p.resetLifes();            // reset life counters
				p.resetScore();           // reset score
                p.setVisible(false);      // will be positioned/shown by loadRoom
                p.setPos(Point(-1, -1));  // keep off-screen until loadRoom runs
            }

            startNewGame();
        }
    }

    // final cleanup
    cleanup_console();
    clrscr();
}