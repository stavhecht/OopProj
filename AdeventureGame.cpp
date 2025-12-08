#include "AdeventureGame.h"
#include "Console.h"

AdeventureGame::AdeventureGame()
    : screen(Screen()) ,players{Player(Point(10, 10, 1, 0, '$', Color::Red),  "wdxase", screen), Player(Point(15, 5,  0, 1, '&', Color::Green), "ilmjko", screen) }{}

void AdeventureGame::init()
{
    init_console();
    clrscr();
    hideCursor();
   
}

void AdeventureGame::run() {
    bool exitApp = false;

    // Outer loop replaced: run until user requests application exit
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
                } else if (key == '2') {
                    // color toggle placeholder
                } else if (key == '3') {
                    screen.setScoreBoard();
                    screen.printBoard();
                    screen.gobacktoMenu();
                } else if (key == '8') {
                    screen.setGuide();
                    screen.printBoard();
                    screen.gobacktoMenu();
                } else if (key == '9') {
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

		
        

        while (running) {
            if (changeRoom) {
                // Set the correct room
                if (currentRoom == 1) {
                    screen.setRoom(currentRoom);
                    Point newStartPos1(10, 10, 1, 0, '$', Color::Red);
                    Point newStartPos2(15, 5, 0, 1, '&', Color::Green);
                    players[0].setPos(newStartPos1);
                    players[1].setPos(newStartPos2);
                    // do item spawns for room 1
                }
                else if (currentRoom == 2) {
                    screen.setRoom(currentRoom);
                    Point newStartPos1(31, 3, 1, 0, '$', Color::Red);
                    Point newStartPos2(34, 3, 0, 1, '&', Color::Green);
                    players[0].setPos(newStartPos1);
                    players[1].setPos(newStartPos2);
                }
                else if (currentRoom == 3) {
                    screen.setRoom(currentRoom);
                    Point newStartPos1(2, 2, 1, 0, '$', Color::Red);
                    Point newStartPos2(2, 4, 0, 1, '&', Color::Green);
                    players[0].setPos(newStartPos1);
                    players[1].setPos(newStartPos2);
                    
                }
                else if (currentRoom > 3) {
                    // Game won!
                    screen.setWin();
                    screen.printBoard();
                    running = false;
                    break;
                }

                // Reset player positions for the new room
                //players[0].setPos(newStartPos1);
                //players[1].setPos(newStartPos2);
                // items spawn in new room

                screen.printRoom(); // Redraw the new room
                changeRoom = false;
            }

            // Player movement and input handling
            for (auto& p : players) {
                p.move();
            }

            // CHECK FOR ROOM TRANSITION AFTER MOVEMENT
            for (Player& p : players) {
                char charAtPlayerPos = p.getScreen().getCharAtcurrentRoom(p.getPos());

                // Check if the character is an exit/door (items '1'..'9')
                if (charAtPlayerPos >= '1' && charAtPlayerPos <= '9') {
                    int doorNum = charAtPlayerPos - '0';

                    if (doorNum == currentRoom) {
                        // Player reached the exit for the CURRENT room
                        currentRoom++;
                        changeRoom = true;
                        // break out of player-for loop to allow changeRoom handling on next top-of-loop
                        break;
                    }
                }
            }

            if (!running)
                break; // Break out of the game loop to trigger menu/cleanup

            // Input handling (Pause menu, etc.)
            if (check_kbhit()) {
                char key = static_cast<char>(get_single_char());
                if (key == ESC) {
                    screen.setGamePaused();
                    screen.printBoard();

                    // Wait for sub-key. Behavior requested:
                    //  - sub == 'H' || 'h' => go back to menu (exit game loop)
                    //  - sub == ESC         => restore view and resume (do NOT assign screen; just redraw)
                    //  - any other key      => remain at pause screen (ignored)
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
                        // Simply redraw current game room and resume.
                        // We DO NOT assign a saved Screen snapshot back into `screen` because that can reintroduce items
                        // that were removed while playing. `screen.printRoom()` will use the live `items` vector and
                        // `currentRoom` buffer (which haven't been overwritten by setGamePaused()) to correctly redraw.
                        screen.printRoom();
                        // continue the game loop normally
                    }
                }
                else {
                    // Normal key handling for players
                    for (auto& p : players) {
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
