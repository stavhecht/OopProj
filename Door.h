#pragma once
#include "SteppedOnItems.h"
#include <vector>
#include <utility>


class Door : public SteppedOnItems {
    bool opened = false;

public:
    enum class OpenMode {
        None,           // open immediately (no key / no switches)
        KeyOnly,        // requires key (legacy)
        SwitchesOnly,   // requires switches in specific states
        KeyAndSwitches  // requires both
    };

private:
    OpenMode openMode = OpenMode::KeyOnly; // default preserve existing behavior
    // Each pair is (groupId == room number, requiredState)
    std::vector<std::pair<int, bool>> switchRequirements;

public:
    Door(const Point& p, char c, Color col = Color::White)
        : SteppedOnItems(p, c, col), opened(false) {
    }

    Door(const Door&) = default;
    Door& operator=(const Door&) = default;

    bool isOpen() const { return opened; }
    void open() { opened = true; }

    void setOpenMode(OpenMode m) { openMode = m; }
    OpenMode getOpenMode() const { return openMode; }

    void setSwitchRequirements(const std::vector<std::pair<int, bool>>& reqs) { switchRequirements = reqs; }
    const std::vector<std::pair<int, bool>>& getSwitchRequirements() const { return switchRequirements; }

    void onStep(Player& player, Screen& screen) override;

    virtual Item* clone() const override { return new Door(*this); }
};
