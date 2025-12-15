#include "AdeventureGame.h"
#include "Bomb.h"
#include "Console.h"

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

void AdeventureGame::loadRoom(int currentRoom, int &movedToNextRoom)
{
    screen.setRoom(currentRoom);

    if (currentRoom == 1) {
        Point newStartPos1(10, 10, 0, 0, '$', Color::Red);
        Point newStartPos2(15, 5, 0, 0, '&', Color::Blue);
        players[0] = newStartPos1;
        players[1] = newStartPos2;
    }
    else if (currentRoom == 2) {
        Point newStartPos1(31, 3, 0, 0, '$', Color::Red);
        Point newStartPos2(34, 3, 0, 0, '&', Color::Blue);
        players[0] = newStartPos1;
        players[1] = newStartPos2;
    }
    else if (currentRoom == 3) {
        Point newStartPos1(2, 2, 0, 0, '$', Color::Red);
        Point newStartPos2(2, 4, 0, 0, '&', Color::Blue);
        players[0] = newStartPos1;
        players[1] = newStartPos2;
    }

    // Make both players visible and clear inventories
    players[0].setVisible(true);
    players[1].setVisible(true);
    for (Player& p : players) {
		CollectableItems* inv = p.takeInventory(); // clear inventory 
        if (inv) {
            delete inv; 
        }
    }

    movedToNextRoom = 0;
    screen.printRoom(); // Redraw the new room
}

void AdeventureGame::processPlayersMovement(int &currentRoom, bool &changeRoom, int &movedToNextRoom, bool &running)
{
    for (auto &p : players) {
        if (!p.isVisible())
            continue;

        p.move();

        Point playerPos = p.getPos();
        // if the player moved onto a previously-opened door tile, make them disappear
        if (screen.isDoorOpenedAt(playerPos)) {
            if (p.isVisible()) {
                p.setVisible(false);
                p.setPos(Point(-1, -1)); // move off-screen
                movedToNextRoom++;

                const int playerCount = static_cast<int>(sizeof(players) / sizeof(players[0]));
                if (movedToNextRoom == playerCount) {
                    currentRoom++;
                    changeRoom = true;
					break; // all players moved to next room
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

        auto res = p.getPos().ItemInRadios(screen, 1); // find nearby items
        bool found = res.first;
        if (!found)
            continue;

        Point itemPos = res.second;
        Item* itemAtPos = screen.peekItemAt(itemPos);
        if (!itemAtPos)
            continue;

        // If it's a stepped-on type, handle accordingly
        SteppedOnItems* sItem = dynamic_cast<SteppedOnItems*>(itemAtPos);
        if (sItem) {
            // Door handling 
            Door* door = dynamic_cast<Door*>(sItem);
            if (door) {
                door->onStep(p, screen);
                // after door handling we do not process other interactions for this tick
                break;
            }

            // Riddle handling
            Riddle* riddle = dynamic_cast<Riddle*>(sItem);
            if (riddle) {
                riddle->onStep(p, screen);
                break;
            }

            break;
        }

		// Collectable items are handled at player::pickUp 
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
    int currentRoom = 1;
    int movedToNextRoom = 0; // track number of players that disappeared for current room
    const int playerCount = static_cast<int>(sizeof(players) / sizeof(players[0]));
    screen.registerPlayers(players, playerCount);

    while (running) {
        screen.updateBombs(); // update pending bombs

        if (changeRoom) {
            if (currentRoom == 1 || currentRoom == 2 || currentRoom == 3) {
                loadRoom(currentRoom, movedToNextRoom);
            }
            else if (currentRoom > 3) {
                // Game won!
                screen.setWin();
                screen.printBoard();
                running = false;
                break;
            }
            changeRoom = false;
        }

        // Player movement and opened doors
        processPlayersMovement(currentRoom, changeRoom, movedToNextRoom, running);
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
