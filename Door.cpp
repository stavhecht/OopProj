#include "Door.h"
#include "Player.h"

void Door::onStep(Player& player, Screen& screen) {
	if (!opened) {
		CollectableItems* inventory = player.getInventory();
		if (inventory && inventory->getCh() == 'K') {
			open();

			// take ownership of the key and delete it (door consumes the key)
			CollectableItems* key = player.takeInventory();
			if (key) {
				delete key;
			}

			// record the door position so future players/logic can detect it
			screen.markDoorOpened(pos);

			// change the room template so the map shows an opened tile.
			// do not (!) remove the Door object from the Screen's item list here,
			// so other code can still find the Door instance and/or rely on the opened registry.
			screen.changePixelInRoom(pos, ' ');
			screen.removeItemAt(pos);
			screen.printRoom();

		}
	}
}

