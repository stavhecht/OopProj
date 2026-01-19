#pragma once

#include <iostream>
#include "Point.h"
#include "CollectableItems.h"
#include "SteppedOnItems.h"
#include "Console.h"
#include "Door.h"
#include "Riddle.h"
#include "Spring.h"
#include "Obstacle.h"
#include "Switcher.h"
#include "Bomb.h"
#include <map>
#include <vector>
#include "Boards.h"
#include <functional>
using namespace std;

class ReplayIO; // forward

class Screen {
public:
	enum { MAX_X = 80, MAX_Y = 25 };
	char currentBoard[MAX_Y][MAX_X + 1] = {}; // +1 for null terminator
	char currentRoom[MAX_Y][MAX_X + 1] = {}; 

	Screen();
	~Screen();

	Screen& operator=(Screen const& other);
	Screen(const Screen& other);

	// color / cell helpers
	void setCellColor(const Point& p, Color c) { if (p.getX() >= 0 && p.getX() < MAX_X && p.getY() >= 0 && p.getY() < MAX_Y) cellColor[p.getY()][p.getX()] = c; }
	Color getCellColor(const Point& p) const { if (p.getX() >= 0 && p.getX() < MAX_X && p.getY() >= 0 && p.getY() < MAX_Y) return cellColor[p.getY()][p.getX()]; return Color::LightYellow; }
	int getCurrentRoomIndex() const { return currentRoomIndex; }

	// Per-room defaults (public)
	void setRoomDefaultColor(int roomIndex, Color c);
	Color getRoomDefaultColor(int roomIndex) const;
	void setRoomUseColor(int roomIndex, bool use);
	bool isRoomUseColor(int roomIndex) const;

	// Save/restore state used for pause/unpause
	void saveStateForPause();
	void restoreStateFromPause();

	// Boards / screens (used by AdeventureGame)
	void setScoreBoard();
	void setMenu();
	void setGuide();
	void setLose();
	void setWin();
	void setGamePaused();
	void setRiddle();
	void setRoom(int nRoom);
	void setErrorBoard(int nRoom);

	//used by loader
	void setRiddlesQA(const map<int, vector<pair<string, string>>>& qa) { riddlesQA = qa; }

	// Print / drawing
	void printBoard(Color col = Color::LightYellow) const;
	void printRoom(Color col = Color::Gray) const;

	// Screen pixel helper (inline)
	void changePixelInRoom(const Point& p, char ch) {
		currentRoom[p.getY()][p.getX()] = ch;
	}

	// Return the visible character at a room location (items override template)
	char getCharAtcurrentRoom(const Point& p) const;

	// Player registration / visibility helpers
	void registerPlayers(Player* players, int count) { registeredPlayers = players; registeredPlayerCount = count; }
	Player* getRegisteredPlayers() { return registeredPlayers; }
	int getRegisteredPlayerCount() const { return registeredPlayerCount; }
	void hidePlayersInRadius(const Point& center, int radius);
	int getVisiblePlayerCount() const;

	// Item / rooms helpers
	Item* getItem(const Point& p);
	void addItem(Item* item) { items.push_back(item); }
	Item* peekItemAt(const Point& p) const;
	bool removeItemAt(const Point & p);
	bool isItem(const Point& p) const;
	bool isDoor(const Point& p) const;
	bool isWall(const Point& p) const {
		return getCharAtcurrentRoom(p) == 'W' || getCharAtcurrentRoom(p) == 'L';
	}

	// Door / opened-door helpers
	void markDoorOpened(const Point& p) { openedDoors.push_back(p); }
	bool isDoorOpenedAt(const Point& p) const;
	void clearOpenedDoors() { openedDoors.clear(); }

	
	void updateBombs();

	// Switch/door evaluation helpers
	bool areSwitchRequirementsSatisfied(const vector<pair<int, bool>>& reqs) const;
	void evaluateDoorRequirements();

	// Convenience: access to raw room source data (used by loader)
	vector<vector<string>>& getGameRoomsData() { return gameRoomsData; }
	const vector<vector<string>>& getGameRoomsData() const { return gameRoomsData; }

	// Riddle UI helper (used by Riddle::onStep)
	void showRiddleQuestion(const string& question);

	// Menu helper (blocks until ESC to return to menu)
	void gobacktoMenu();


private:
	// Live runtime state
	vector<Item*> items;
	vector<Point> openedDoors;
	Player* registeredPlayers = nullptr;
	int registeredPlayerCount = 0;
	int currentRoomIndex = 0;
	vector<vector<string>> gameRoomsData;
	// Riddles QA storage (injected via setRiddlesQA)
	map<int, vector<pair<string, string>>> riddlesQA;

	// Per-cell color buffer
	Color cellColor[MAX_Y][MAX_X];

	// Per-room defaults
	static constexpr int ROOM_COUNT = 3;
	Color roomDefaultColor[ROOM_COUNT];
	bool roomUseColor[ROOM_COUNT];

    // Saved pause state grouped in ScreenState
    struct ScreenState {
        char room[MAX_Y][MAX_X + 1];
        Color colors[MAX_Y][MAX_X];
        bool has = false;
    } saved;

	// --- private helpers ---
	void clearAndDeleteItems();
	void populateLiveItemsFromRoom(int nRoom);
	void applyRoomDefaultColors(int nRoom);
	void writeTextToBoard(int row, int col, const string& text);

	// Riddle getters — used only inside Screen
	const vector<pair<string, string>>& getRiddlesForRoom(int roomIndex1Based) const;

	string getRiddleQuestion(int roomIndex1Based) const;

	string getRiddleAnswer(int roomIndex1Based) const;

	// Score helpers — only internal use (players hold the real scores)
	void addScoreToPlayer(int playerIndex, int delta);
	int getPlayerScore(int playerIndex) const;
	void resetAllScores();

	// UI helper used only from printRoom
	void printPlayersinfo() const;

    // Templates moved to Boards namespace (see Boards.h)

    // Replay hooks (no globals) - optional pointer owned externally by caller
    ReplayIO* replayIo = nullptr;
    int replayTick = 0;

public:
    void setReplayIO(ReplayIO* io) { replayIo = io; }
    ReplayIO* getReplayIO() const { return replayIo; }
    void setReplayTick(int t) { replayTick = t; }
    int getReplayTick() const { return replayTick; }
    // Convenience accessors used by other modules (Riddle, AdeventureGame)
    // keep names consistent

};

