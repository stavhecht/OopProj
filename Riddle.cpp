#include "Riddle.h"
#include "Console.h"

void Riddle::onStep(Player& player, Screen& screen)  {
	if (answered)
		return;

	screen.setRiddle();
	screen.printBoard();
	const int posY = 15;
	const int posX = 40;

	while (true) {
		gotoxy(posX, posY);
		char answer = get_single_char();
		std::cout << answer << std::flush;
		
		if (answer == '4') { // correct answer
			sleep_ms(500);
			answered = true;
			Point p = pos;
			screen.changePixelInRoom(pos, ' ');
			pos.setCh(' ');
			screen.printRoom();
			screen.removeItemAt(p);
			return;
		}
		else if (answer == 27) {
			screen.printRoom();
			return;
		}
		else {
			gotoxy(posX, posY);
			std::cout << static_cast<char>(answer) << std::flush;

			// provide brief feedback for wrong answer, then clear it so user can retry
			gotoxy(posX, posY);
			

			sleep_ms(500);

			// erase typed char and feedback
			gotoxy(posX, posY);
			std::cout << ' ' << std::flush;
			
		}
	}


}