#include "Door.h"
#include "Player.h"
#include "Screen.h"
#include "CollectableItems.h"

void Door::onStep(Player& player, Screen& screen) {
    if (opened)
        return;

    // Evaluate switch requirements (if any)
    bool switchesSatisfied = true;
    if (!switchRequirements.empty()) {
        switchesSatisfied = screen.areSwitchRequirementsSatisfied(switchRequirements);
    }

    switch (openMode) {
    case OpenMode::None:
        open();
        break;

    case OpenMode::SwitchesOnly:
        if (!switchesSatisfied) return;
        open();
        break;

    case OpenMode::KeyOnly: {
        CollectableItems* inventory = player.getInventory();
        if (!(inventory && inventory->getCh() == 'K')) {
            // missing key -> cannot open
            return;
        }
        // consume key and open
        open();
        CollectableItems* key = player.takeInventory();
        if (key) delete key;
        break;
    }

    case OpenMode::KeyAndSwitches: {
        if (!switchesSatisfied) return;
        CollectableItems* inventory = player.getInventory();
        if (!(inventory && inventory->getCh() == 'K')) {
            // missing key -> cannot open
            return;
        }
        open();
        CollectableItems* key = player.takeInventory();
        if (key) delete key;
        break;
    }
    }

    if (opened) {
        // persist change to screen and visuals
        screen.markDoorOpened(pos);
    }
}

