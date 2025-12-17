#include "Switcher.h"

void Switcher::onStep(Player& player, Screen& screen) {
    if (onOff) {  // on -> off
        onOff = false;
        pos.setCh('/');
        pos.setColor(Color::Red);
    } else {      // off -> on
        onOff = true;
        pos.setCh('\\');
        pos.setColor(Color::LightGreen);
    }

    // update template and color buffer, redraw
    screen.changePixelInRoom(pos, pos.getCh());
    screen.setCellColor(pos, pos.getColor());
    screen.printRoom();

    // Notify screen to evaluate door rules that may depend on switch state.
    screen.evaluateDoorRequirements();
}
