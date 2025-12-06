#include "Riddle.h"

void Riddle::onStep(Player& player, Screen& screen)  {
	if (answered)
		return;


	screen.setRiddle();
	screen.printBoard();
	const int posY = 16;
	const int posX = 40;

	while (true) {
		gotoxy(posY, posX);
		char answer = get_single_char();
		if (answer == '4') { // correct answer
			answered = true;
			screen.changePixelInRoom(pos, ' ');
			screen.printRoom();
			return;
		}
		else if (answer == 27) {
			screen.printRoom();
			return;
		}
		else {
			gotoxy(posY, posX + 1);
			std::cout << '/b' << std::flush;
		}
	}


}