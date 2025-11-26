#include "AdeventureGame.h"
#include "Console.h"

AdeventureGame::AdeventureGame()
    : players{
        Player(Point(10, 10, 1, 0, '$', Color::Red),  "wdxas", screen),
        Player(Point(15, 5,  0, 1, '&', Color::Green), "ilmjk", screen)
    }
{
}

void AdeventureGame::init()
{
    init_console();
    clrscr();
    hideCursor();
    screen.draw();
}

void AdeventureGame::run() {
    while (true) {
        for (auto& p : players) {
            p.move();
        }

        if (check_kbhit()) {
            char key = static_cast<char>(get_single_char());
            if (key == ESC) {
                key = static_cast<char>(get_single_char());
                if (key == 'H' || key == 'h') {
                    break;
                }
            }
            else {
                for (auto& p : players) {
                    p.handleKeyPressed(key);
                }
            }
        }

        sleep_ms(100);
    }

    cleanup_console();
    clrscr();
}
