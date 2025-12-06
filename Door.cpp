#include "Door.h"
#include "Player.h"

void Door::onStep(Player& player, Screen& screen) {
	if (!opened) {
		CollectableItems* inventory = player.getInventory();
		if (inventory && inventory->getCh() == 'K') {
			open();
			player.takeInventory(); // remove key from player
			screen.changePixelInCurrBoard(pos, ' '); // open the door visually
		}
	}
}

