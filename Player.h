#pragma once

#include "Point.h"
#include "CollectableItems.h"
#include "Screen.h"

class Player {
	static constexpr int NUM_KEYS = 6;
	Point pos;
	char keys[NUM_KEYS];
	Screen& screen;
	CollectableItems* inventory = nullptr;
	bool visible = true;

	// spring related
	int launchSpeed = 0;
	int launchRemaining = 0;
	Direction launchDir = Direction::STAY;

public:
	// ctor gets start point and an array of 5 keys
	Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& room);

	void draw();
	void move();
	void pickUp();
	CollectableItems* dispose();

	// Return current inventory pointer (may be nullptr)
	CollectableItems* getInventory() const { return inventory; }

	// Take (remove) the inventory from the player and return it (player no longer owns it).
	// Caller becomes responsible for the returned pointer. This is used when a door consumes a key.
	CollectableItems* takeInventory();

	void handleKeyPressed(char key);

	Point getPos() const { return pos; }
	// setPos preserves player's glyph/color; use operator=(Point) to replace entire appearance when needed
	void setPos(const Point& p);
	void setInventory(CollectableItems* item) { inventory = item; }

	Screen& getScreen() { return screen; }
	const Screen& getScreen() const { return screen; }

	// Visibility helpers
	void setVisible(bool v);
	bool isVisible() const { return visible; }

	Player& operator=(const Point& p){pos = p; return *this;}
	Player& operator=(const Player& other);

	// Launch API used by springs and collisions
	void applyLaunch(int speed, Direction dir, int duration);
	int currentForce() const { return (launchRemaining > 0 && launchSpeed > 0) ? launchSpeed : 1; }

private:
	size_t findKeyIndex(char key_pressed) const;

	void handlePickupOrDispose(size_t index);

	void setDirectionByIndex(size_t index);

	bool canMoveTo(const Point& next) const;
	void updateTorchOnMove(const Point& next);

	static Point nextPointFor(const Player& p);
	static Point stepPointFrom(const Point& pt, Direction dir);
	static bool otherIsPushingTowards(const Player& other, const Point& target);
	static int computeCombinedForceForPush(Screen& screen, const Player& self, const Point& obstacleCell);
	static bool tryPerformPush(Screen& screen, Player& player, const Point& next, Direction pushDir, int combinedForce, int launchSpeedIfAny, int* launchRemainingRef = nullptr);
};
