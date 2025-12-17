#include "AdeventureGame.h"
#include "Console.h"
#include <vector>

AdeventureGame::AdeventureGame()
    : screen(Screen()), players{ Player(Point(10, 10, 1, 0, '$', Color::Red), "wdxase", screen),
                                Player(Point(15, 5, 0, 1, '&', Color::Blue), "ilmjko", screen) } {}

void AdeventureGame::init()
{
    init_console();
    clrscr();
    hideCursor();
}



bool AdeventureGame::waitForMenuSelection(bool &exitApp)
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

void AdeventureGame::loadRoom(int currentRoom, const std::vector<bool>& playersMoved)
{
    screen.setRoom(currentRoom);

    // default start positions for players in each room
    Point startPos[2];
    if (currentRoom == 1) {
        startPos[0] = Point(10, 10, 0, 0, '$', Color::Red);
        startPos[1] = Point(15, 5, 0, 0, '&', Color::Blue);
    }
    else if (currentRoom == 2) {
        startPos[0] = Point(31, 3, 0, 0, '$', Color::Red);
        startPos[1] = Point(34, 3, 0, 0, '&', Color::Blue);
    }
    else if (currentRoom == 3) {
        startPos[0] = Point(2, 2, 0, 0, '$', Color::Red);
        startPos[1] = Point(2, 4, 0, 0, '&', Color::Blue);
    } else {
        startPos[0] = Point(1,1,0,0,'$', Color::Red);
        startPos[1] = Point(2,1,0,0,'&', Color::Blue);
    }

    // clear inventories for all players
    for (Player& p : players) {
        CollectableItems* inv = p.takeInventory();
        if (inv) delete inv;
    }

    // If playersMoved is empty or all false -> spawn all players.
    bool anyMoved = false;
    for (bool b : playersMoved) if (b) { anyMoved = true; break; }

    const int playerCount = static_cast<int>(sizeof(players) / sizeof(players[0]));
    for (int i = 0; i < playerCount; ++i) {
        if (!anyMoved) {
            // initial spawn: enable both players
            players[i] = startPos[i];
            players[i].setVisible(true);
        } else {
            if (i < static_cast<int>(playersMoved.size()) && playersMoved[i]) {
                // only spawn players that moved
                players[i] = startPos[i];
                players[i].setVisible(true);
            } else {
                // hide players that didn't move
                players[i].setVisible(false);
                players[i].setPos(Point(-1, -1));
            }
        }
    }

    screen.printRoom(); // Redraw the new room
}

void AdeventureGame::processPlayersMovement(int &currentRoom, bool &changeRoom, std::vector<bool> &playersMoved, bool &running)
{
    const int playerCount = static_cast<int>(sizeof(players) / sizeof(players[0]));

    for (int i = 0; i < playerCount; ++i) {
        Player &p = players[i];
        if (!p.isVisible())
            continue;

        p.move();

        Point playerPos = p.getPos();
        // if the player moved onto a previously-opened door tile, mark them moved and hide
        if (screen.isDoorOpenedAt(playerPos)) {
            if (p.isVisible()) {
                p.setVisible(false);
                p.setPos(Point(-1, -1)); // move off-screen
                if (i >= static_cast<int>(playersMoved.size()))
                    playersMoved.resize(playerCount, false);
                playersMoved[i] = true;

                // Advance room when there are no visible players left.
                if (screen.getVisiblePlayerCount() == 0) {
                    currentRoom++;
                    changeRoom = true;
                    break; // all remaining players moved to next room / none remain
                }
            }
        }
    }
}

void AdeventureGame::processSteppedOnInteractions(bool &changeRoom)
{
    for (auto &p : players) {
        if (!p.isVisible())
            continue;

        Point playerPos = p.getPos();

        
        auto adj = playerPos.SteppedOnAdjacent(screen);
        if (adj.first) {
            Item* adjItem = screen.peekItemAt(adj.second);
            if (adjItem) {
                SteppedOnItems* sItemAdj = dynamic_cast<SteppedOnItems*>(adjItem);
                if (sItemAdj) {
                    Door* door = dynamic_cast<Door*>(sItemAdj);
                    if (door) {
                        door->onStep(p, screen);
                        continue;
                    }

                    Riddle* riddle = dynamic_cast<Riddle*>(sItemAdj);
                    if (riddle) {
                        riddle->onStep(p, screen);
                        continue;
                    }

                    Switcher* switcher = dynamic_cast<Switcher*>(sItemAdj);
                    if (switcher) {
                        switcher->onStep(p, screen);
                        continue;
                    }

                    // other SteppedOnItems
                    sItemAdj->onStep(p, screen);
                    continue;
                }
            }
        }
    } 
}

void AdeventureGame::handleInput(bool &running, bool &changeRoom)
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
        for (auto &p : players) {
            if (p.isVisible())
                p.handleKeyPressed(key);
        }
    }
}

void AdeventureGame::startNewGame()
{
    bool running = true;
    bool changeRoom = true;
    int currentRoom = 3;
    const int playerCount = static_cast<int>(sizeof(players) / sizeof(players[0]));
    std::vector<bool> playersMoved(playerCount, false);
    screen.registerPlayers(players, playerCount);

    while (running) {
        screen.updateBombs(); // update pending bombs
        

        if (changeRoom) {
            if (currentRoom == 1 || currentRoom == 2 || currentRoom == 3) {
                loadRoom(currentRoom, playersMoved);
            }
            else if (currentRoom > 3) {
                // Game won!
                screen.setWin();
                screen.printBoard();
                running = false;
                while(true) {
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
            std::fill(playersMoved.begin(), playersMoved.end(), false);
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
            while(true) {
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
    bool exitApp = false;

    while (!exitApp) {
        bool startGame = waitForMenuSelection(exitApp);
        if (exitApp) {
            break;
        }
        if (startGame) {
            startNewGame();
        }
    }

    // final cleanup
    cleanup_console();
    clrscr();
}
