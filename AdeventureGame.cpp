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

        Point newStartPos1(10, 10, 1, 0, '$', Color::Red);
        Point newStartPos2(15, 5, 0, 1, '&', Color::Green);


        while (running) {
            if (changeRoom) {
                // Set the correct room
                if (currentRoom == 1) {
                    screen.setRoom1();
                }
                else if (currentRoom == 2) {
                    screen.setRoom2();
                }
                else if (currentRoom == 3) {
                    screen.setRoom3();
                }
                else if (currentRoom > 3) {
                    // Game won!
                    screen.setWin();
                    screen.printBoard();
                    running = false;
                    break;
                }

                // Reset player positions for the new room
                players[0].setPos(newStartPos1);
                players[1].setPos(newStartPos2);

                screen.printRoom(); // Redraw the new room
                changeRoom = false;
            }

            // Player movement and input handling
            for (auto& p : players) {
                p.move();
            }

            // CHECK FOR ROOM TRANSITION AFTER MOVEMENT
            for (Player& p : players) {
                char charAtPlayerPos = p.getScreen().getCharFromOriginalRoom(p.getPos());

                // Check if the character is an exit/door (items '1', '2', '3')
                if (charAtPlayerPos >= '1' && charAtPlayerPos <= '9') {
                    int doorNum = charAtPlayerPos - '0';

                    if (doorNum == currentRoom) {
                        // Player reached the exit for the CURRENT room (e.g., reached '1' in Room 1)
                        // This logic assumes door '1' leads out of room 1, '2' out of room 2, etc.
                        currentRoom++;
                        changeRoom = true;

                        // We must break the inner loops to start the room change immediately
                        running = false;
                        break;
                    }
                }
            }

            if (!running) break; // Break out of the game loop to trigger room change or menu return

            // Input handling (Pause menu, etc. - unchanged)
            if (check_kbhit()) {
                char key = static_cast<char>(get_single_char());
                if (key == ESC) {
                    Screen lastScreen = screen;
                    screen.setGamePaused();
                    screen.printBoard();
                    char sub = static_cast<char>(get_single_char());
                    if (sub == 'H' || sub == 'h') {
                        running = false;
                        break;
                    }
                    if (sub == ESC) {
                        screen = lastScreen;
                        screen.printRoom();
                    }
                    else {
                        screen = lastScreen;
                        for (auto& p : players) {
                            p.handleKeyPressed(sub);
                        }
                    }
                }
                else {
                    for (auto& p : players) {
                        p.handleKeyPressed(key);
                    }
                }
            }

            sleep_ms(100);
        } // End while(running)

        // If the game loop broke because of a room change (running is false, but exitApp is false)
        // the outer loop will restart the game loop with the new nRoom/changeRoom state.
    } // End while(!exitApp)

    // final cleanup
    cleanup_console();
    clrscr();
}
