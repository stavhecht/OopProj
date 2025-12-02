#include "AdeventureGame.h"
#include "Console.h"

AdeventureGame::AdeventureGame()
    : screen(Screen()) ,players{Player(Point(10, 10, 1, 0, '$', Color::Red),  "wdxas", screen), Player(Point(15, 5,  0, 1, '&', Color::Green), "ilmjk", screen) }{}

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
        screen.print();

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
                    screen.print();
                    screen.gobacktoMenu();
                } else if (key == '8') {
                    screen.setGuide();
                    screen.print();
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
        while (running) {
            for (auto& p : players) {
                p.move();
            }

            if (check_kbhit()) {
                char key = static_cast<char>(get_single_char());
                if (key == 27) { // ESC pressed -> pause menu
                    Screen lastScreen = screen;      // save current screen to restore if needed
                    screen.setGamePaused();
                    screen.print();

                    // wait for a keypress while paused
                    while (!check_kbhit()) {
                        sleep_ms(50);
                    }
                    char sub = static_cast<char>(get_single_char());

                    if (sub == 'H' || sub == 'h') {
                        // go back to main menu
                        screen = lastScreen; // restore state (if needed)
                        running = false;     // break out of game loop and return to menu
                        break;
                    }
                    if (sub == 27) {
                        // ESC while paused -> resume
                        screen = lastScreen;
                        screen.print();
                    } else {
                        // treat other input while paused as an immediate action (e.g. resume + key)
                        screen = lastScreen;
                        for (auto& p : players) {
                            p.handleKeyPressed(sub);
                        }
                    }
                } else {
                    // regular in-game key handling
                    for (auto& p : players) {
                        p.handleKeyPressed(key);
                    }
                }
            }

            sleep_ms(100);
        }

        // After game loop finishes, flow returns to menu (outer while)
    }

    // final cleanup
    cleanup_console();
    clrscr();
}
