#include "AdeventureGame.h"
#include "Bomb.h"
#include "Console.h"

AdeventureGame::AdeventureGame()
    : screen(Screen()), players{ Player(Point(10, 10, 1, 0, '$', Color::Red), "wdxase", screen),
                                Player(Point(15, 5, 0, 1, '&', Color::Green), "ilmjko", screen) } {}

void AdeventureGame::init()
{
    init_console();
    clrscr();
    hideCursor();
}

void AdeventureGame::run() {
    bool exitApp = false;

    // Outer loop: run until user requests application exit
    while (!exitApp) {
        // show menu and wait for selection
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

        if (exitApp) {
            break;
        }

        // Game loop
        bool running = true;
        bool changeRoom = true;
        int currentRoom = 1;
        int movedToNextRoom = 0; // track number of players that disappeared for current room
        const int playerCount = static_cast<int>(sizeof(players) / sizeof(players[0]));
        screen.registerPlayers(players, playerCount);


        while (running) {
            screen.updateBombs(); // update pending bombs
            if (changeRoom) {
                // Set the correct room
                if (currentRoom == 1) {
                    screen.setRoom(currentRoom);
                    Point newStartPos1(10, 10, 0, 0, '$', Color::Red);
                    Point newStartPos2(15, 5, 0, 0, '&', Color::Green);
					players[0] = newStartPos1;
					players[1] = newStartPos2;
                    players[0].setVisible(true);
                    players[1].setVisible(true);
                    // clear player inventories safely
                    for (Player& p : players) {
                        CollectableItems* inv = p.takeInventory(); // safely take ownership out of the player
                        if (inv) {
                            delete inv; // discard item (no leak) — player starts with empty inventory
                        }
                    }

                    movedToNextRoom = 0; // reset counter for the new room
                }
                else if (currentRoom == 2) {
                    screen.setRoom(currentRoom);
                    Point newStartPos1(31, 3, 0, 0, '$', Color::Red);
                    Point newStartPos2(34, 3, 0, 0, '&', Color::Green);
                    players[0] = newStartPos1;
                    players[1] = newStartPos2;
                    players[0].setVisible(true);
                    players[1].setVisible(true);
                    movedToNextRoom = 0;
                }
                else if (currentRoom == 3) {
                    screen.setRoom(currentRoom);
                    Point newStartPos1(2, 2, 0, 0, '$', Color::Red);
                    Point newStartPos2(2, 4, 0, 0, '&', Color::Green);
                    players[0] = newStartPos1;
                    players[1] = newStartPos2;
                    players[0].setVisible(true);
                    players[1].setVisible(true);
                    movedToNextRoom = 0;
                }
                else if (currentRoom > 3) {
                    // Game won!
                    screen.setWin();
                    screen.printBoard();
                    running = false;
                    break;
                }

                screen.printRoom(); // Redraw the new room
                changeRoom = false;
            }

            // Player movement and immediate disappearance check (opened doors)
            for (auto& p : players) {
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

                        if (movedToNextRoom == playerCount) {
                            currentRoom++;
                            changeRoom = true;
                            break; // stop processing players; top-of-loop will handle room change
                        }
                    }
                }
            }

            if (changeRoom) {
                // reload next room on next iteration
                continue;
            }

            // CHECK FOR STEPPED-ON INTERACTIONS AFTER MOVEMENT
            for (auto& p : players) {
                if (!p.isVisible())
                    continue;

                auto res = p.getPos().ItemInRadios(screen, 1); // find nearby stepped-on items or collectables
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
                    // Door handling (doors are SteppedOnItems-derived)
                    Door* door = dynamic_cast<Door*>(sItem);
                    if (door) {
                        Point doorPos = door->getPos();
                        door->onStep(p, screen);
                        // after door handling we do not process other interactions for this tick
                        break;
                    }

                    // Riddle handling (example)
                    Riddle* riddle = dynamic_cast<Riddle*>(sItem);
                    if (riddle) {
                        riddle->onStep(p, screen);
                        if (riddle->isAnswered()) {
                            //screen.removeItemAt(itemPos);
                        }
                        break;
                    }


                
                    break;
                }

                // Collectable items are picked up via player's pickUp key — no automatic handling here.
            } // end for players (interactions)

            if (changeRoom) {
                // next loop iteration will reload room
                continue;
            }

            if (!running)
                break; // Break out of the game loop to trigger menu/cleanup

            // Input handling (Pause menu, etc.)
            if (check_kbhit()) {
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
                        break; // break the while(running) loop
                    }

                    if (requestUnpause) {
                        // Restore prior room + colors and resume.
                        screen.restoreStateFromPause();
                        screen.printRoom();
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

            sleep_ms(100);
        } // End while(running)

        // If the game loop ended (running == false) control returns here and the outer
        // while(!exitApp) will show the menu again (or exit if exitApp set).
    } // End while(!exitApp)

    // final cleanup
    cleanup_console();
    clrscr();
}
