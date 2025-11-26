#include <iostream>
#include "Snake.h"
#include "Screen.h"
#include "console.h"

enum Keys { ESC = 27 };

int main() {
	init_console();
	clrscr();
	hideCursor();
	Screen screen;
	screen.draw();
	Snake snakes[] = {
		Snake(Point(10, 10, 1, 0, '@', Color::Red), "wdxas", screen),
		Snake(Point(15, 5, 0, 1, '*', Color::Green), "ilmjk", screen)
	};
	
	// this line is need in order to view the screens in the console properly.
	//gotoxy(0, 25);
	//reset_color();

	while (true) {
		for(auto& s : snakes) {
			s.move();
		}
		if (check_kbhit()) {
			
			char key = get_single_char();
			if (key == Keys::ESC) {
				// pause, wait for any key to continue
				key = get_single_char();
				if (key == 'H' || key == 'h') {
					break; // end the game
				}
			}
			else {
				for (auto& s : snakes) {
					s.handleKeyPressed(key);
				}
			}
		}
		sleep_ms(50);
	}
	cleanup_console();
	clrscr();
}