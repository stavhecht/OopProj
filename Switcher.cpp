#include "Switcher.h"

void Switcher::onStep(Player& player, Screen& screen) {
	if (onOff) {  // switcher on -> turn off
		onOff = false;
		pos.setCh('/');
		pos.setColor(Color::Red);
	} else {      // switcher off -> turn on
		onOff = true;
		pos.setCh('\\'); 
		pos.setColor(Color::LightGreen);
	}

	// update room template char and the per-cell color buffer, then redraw
	screen.changePixelInRoom(pos, pos.getCh());
	screen.setCellColor(pos, pos.getColor());
	screen.printRoom();
}
