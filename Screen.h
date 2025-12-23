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
using namespace std;



class Screen {
public:
	enum { MAX_X = 80, MAX_Y = 25 };
	char currentBoard[MAX_Y][MAX_X + 1] = {}; // +1 for null terminator
	char currentRoom[MAX_Y][MAX_X + 1] = {}; 

private:
	vector<Item*> items;
	vector<Point> openedDoors;
	Player* registeredPlayers = nullptr;
	int registeredPlayerCount = 0;
	int currentRoomIndex = 0;

	// Per-cell color buffer (keeps illumination persistent across redraws & pauses)
	Color cellColor[MAX_Y][MAX_X];

	// Per-room default color data (index 1..3 used for rooms)
	static constexpr int ROOM_COUNT = 3; // index 0 unused
	Color roomDefaultColor[ROOM_COUNT];
	bool roomUseColor[ROOM_COUNT]; // when false, room will be "uncolored" (uses uncolored default)

	// Saved state when entering pause (so we can restore it exactly)
	char savedRoom[MAX_Y][MAX_X + 1];
	Color savedCellColor[MAX_Y][MAX_X];
	bool hasSavedState = false;

	// --- private helpers (extracted to keep setRoom / ctor logic clearer) ---
	void clearAndDeleteItems();
	void copyTemplateToCurrentRoom(const char* const roomTemplate[]);
	void populateLiveItemsFromRoom(int nRoom);   // <-- updated: accept room number
	void applyRoomDefaultColors(int nRoom);

	const char* gameRoom1[MAX_Y] = {
		//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                          W    WWWW", // 1
			"WWWWW              WWWWWWWWWW                                          W    WWWW", // 2
			"WWWWW                                                                  W    WWWW", // 3
			"WWWWW                                                                  W    WWWW", // 4
			"WWWWW                                                                  W    WWWW", // 5
			"WWWWW                                                                  W    WWWW", // 6
			"WWWWW                                                                       WWWW", // 7
			"WWWWW                WWWWWWWW                                               WWWW", // 8
			"WWWWW                WWWWWWWW                                               WWWW", // 9
			"WWWWW                                                                         1 ", // 10
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                               WWWW", // 11
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                               WWWW", // 12
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                          W    WWWW", // 13
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                          W    WWWW", // 14
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW?WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 15
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWWWWWWWWWW", // 16
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWWWWWWWWWW", // 17
			"WWWWWWWWWWWWWWW     WWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWWWWWWWWWW", // 18
			"WWWWWWWWWWWWWWW K   WWW                                 WWWWWWWWWWWWWWWWWWWWWWWW", // 19
			"WWWWWWWWWWWWWWW     WWWWWWWWWWWWWWW          @          WWWWWWWWWWWWWWWWWWWWWWWW", // 20
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWWWWWWWWWW", // 21
			"WWWWWWLWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 22
			"W                                                                              W", // 23
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};

	// tabs removed and layout normalized; lines may be shorter than MAX_X but Screen::setRoomX will pad safely.
	const char* gameRoom2[MAX_Y] = {
	//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		"WWWWWWWWWWWWW   @WWWW                  WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 1
		"WWWWWWWWWWWW/    WWWWW                            WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 2
		"WWWWWWWWWWWWW    WWWWW  WW                        WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 3
		"WWWWWWWWWWWWW    WWWWW  WW            WW          WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 4
		"WWWWWWWWWWWWW    WWWWWWWWWWWW         WW          WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 5
		"WWWWWWWWWWWWW    WWWWWWWWWWWW        WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 6
		"W                   WWWW                            WWWW                       W", // 7
		"W                                                   WWWW                       W", // 8
		"W                                                   WWWW                       W", // 9
		"W         WW        WWWWWWWW                        WWWW                       W", // 10
		"W         WW        WWWWWWWW                        WWWW                       W", // 11
		"W         WW        WWWWWWWWWWWWWWWWWWWWWWWWWWW/WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 12
		"W         WW        WWWWWWWW                        WWW              W        WW", // 13
		"W         WWWWWWWWWWWWWWW                           WWW              W        WW", // 14
		"W                      WW                           /WW              W        /W", // 15
		"W                      WW                           WWW              W        WW", // 16
		"W             WWWW   WWWW                    WWWWWWWWWW              W        WW", // 17
		"W             W##  ** WWWWWWWWWWWWWWWWWWWWWWWWW/WWWWWWWWWWWWWWWWWWWWWW        WW", // 18
		"W             WWWWW    W   W W   W   W   W   W              W  W    WW        2W", // 19
		"W             WWWWW      W W W W W W W W W W WWWWWW   WWWWW W  W  W WW        WW", // 20
		"WWWWWWWWWWWWWWWWWWW      W     W   W   W   W          WWWWW       W           WW", // 21
		"WWWWWWLWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 22
		"W                                                                              W", // 23
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24

	};

	
	const char* gameRoom3[MAX_Y] = {
		//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
			"W                 W                          W                                 W", // 1 
			"W  WWWWWWWWWWWW   W   WWWWWW   WWWWWWWW      W   WWWWWWWWWWWWWWWWWW            W", // 2 
			"W  W              W       W          W        W   W              W             W", // 3  
			"W  W   WWWWWW W  !WWWWW   WWWWWWW    W   WWWWWW   W   WWWWWWWW   W   WWWWWW    W", // 4  
			"W  W       W  W                 W    W       W    W        W     W       W     W", // 5 
			"W  WWWWW   W  WWWWWWWWWW   WWW  WWWWWW   W   W    WWWWWWW  W  WWWWWWWW  W  WWWWW", // 6  
			"W      W   W          W     W      W     W   W          W  W        W  W       W", // 7 
			"WWWWW  WWWWW   WWWWW  W  WWWWWWW   WWWWWWW   WWWWW   WWWW  WWWWWWW  WWW   WWWWWW", // 8  
			"W                  W  W        W                 W        W       W            W", // 9  
			"W  WWWWWWWWW   W   W  WWWWWWW  WWWWWWWWWWWWWWW   WWWWWWW  WWWWWWW WWWWWWWWWW   W", // 10 
			"W  W        W  W   W        W                 W     K          W        W      W", // 11 
			"W  W   WWWWWW  WWWWW  WWWWWWW  WWWWWWWWWWWWW  WWWWWW   WWWWW   WWWWW   W   WWW W", // 12 
			"W !W       W        W        W    @         W       W       W       W  W      WW", // 13 
			"W  WWWWW   W  WWWW  WWWWWW   WWWWWWWWWW     WWWWW   WWWWW   WWWWW   WWWWWWWW  WW", // 14 
			"W      W  W      W        W          W         W        W        W          W  W", // 15 
			"WWWWW  WWWWWWWW  W  WWWWWWW  WWWWWWWW  WWWWWWWW  WWWWW  WWWWWW  WWWWWWWW   WWWWW", // 16 
			"W         W      W       W        W    @       W      W        W               W", // 17 
			"W   WWWW  W  WWWWW  WWW  WWWWWWW  WWWWWWWWWW   WWWWW  WWWWWW  WWWWWWWW   WWWWW W", // 18
			"W         W             W           W                 W        W               W", // 19 
			"W   WWW   W   WWWWWWW   WWWWWWW    W   WWWWWWWWWW     WWWWWWW  W   WWWWWWWWWW  W", // 20 
			"W       W                 W        W           W              W             WW3W", // 21 
			"WWWWWWLWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 22
			"W                                                                              W", // 23
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};


	const char* winBoard[MAX_Y] = {
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "W==============================================================================W", // 1
		 "W                                                                              W", // 2
		 "W                                                                              W", // 3
		 "W                                                                              W", // 4
		 "W                                                                              W", // 5
		 "W                   ||    You Completed all the stages    ||                   W", // 6
		 "W                                                                              W", // 7
		 "W                                                                              W", // 8
		 "W                                Your Score:                                   W", // 9
		 "W                                                                              W", // 10
		 "W                                                                              W", // 11
		 "W                                                                              W", // 12
		 "W                                                                              W", // 13
		 "W                                                                              W", // 14
		 "W                                                                              W", // 15
		 "W                                                                              W", // 16
		 "W                                                                              W", // 17
		 "W                                                                              W", // 18
		 "W                                                                              W", // 19
		 "W                                                                              W", // 20
		 "W                        Press ESC to go back to the menu                      W", // 21
		 "W                                                                              W", // 22
		 "W==============================================================================W", // 23
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};
	const char* scoreBoard[MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "W==============================================================================W", // 1
		 "W                                                                              W", // 2
		 "W                                                                              W", // 3
		 "W                            ||  SCORE BOARD  ||                               W", // 4
		 "W                                                                              W", // 5
		 "W                                                                              W", // 6
		 "W                                                                              W", // 7
		 "W                                                                              W", // 9
		 "W                                                                              W", // 9
		 "W                                                                              W", // 10
		 "W                                                                              W", // 11
		 "W                                                                              W", // 12
		 "W                                                                              W", // 13
		 "W                                                                              W", // 14
		 "W                                                                              W", // 15
		 "W                                                                              W", // 16
		 "W                                                                              W", // 17
		 "W                                                                              W", // 18
		 "W                                                                              W", // 19
		 "W                                                                              W", // 20
		 "W                        Press ESC to go back to the menu                      W", // 21
		 "W                                                                              W", // 22
		 "W==============================================================================W", // 23
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};
	const char* screenErrorBoard[MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "W==============================================================================W", // 1
		 "W                                                                              W", // 2
		 "W                                                                              W", // 3
		 "W                                                                              W", // 4
		 "W                                                                              W", // 5
		 "W                                                                              W", // 6
		 "W                            Screen #  is not good                             W", // 7
		 "W                                                                              W", // 9
		 "W                                                                              W", // 9
		 "W                                                                              W", // 10
		 "W                                                                              W", // 11
		 "W                      Press ENTER to go to the next stage                     W", // 12
		 "W                                                                              W", // 13
		 "W                                                                              W", // 14
		 "W                                                                              W", // 15
		 "W                                                                              W", // 16
		 "W                                                                              W", // 17
		 "W                                                                              W", // 18
		 "W                                                                              W", // 19
		 "W                                                                              W", // 20
		 "W                                                                              W", // 21
		 "W                                                                              W", // 22
		 "W==============================================================================W", // 23
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};
	const char* loseBoard[MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "W==============================================================================W", // 1
		 "W                                                                              W", // 2
		 "W                                                                              W", // 3
		 "W                                                                              W", // 4
		 "W                                                                              W", // 5
		 "W                          ||     GAME   OVER     ||                           W", // 6
		 "W                                                                              W", // 7
		 "W                                                                              W", // 9
		 "W                                  Your Score:                                 W", // 9
		 "W                                                                              W", // 10
		 "W                                                                              W", // 11
		 "W                                                                              W", // 12
		 "W                                                                              W", // 13
		 "W                                                                              W", // 14
		 "W                                                                              W", // 15
		 "W                                                                              W", // 16
		 "W                                                                              W", // 17
		 "W                                                                              W", // 18
		 "W                                                                              W", // 19
		 "W                                                                              W", // 20
		 "W                        Press ESC to go back to the menu                      W", // 21
		 "W                                                                              W", // 22
		 "W==============================================================================W", // 23
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};


	const char* riddleBoard[MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "????????????????????????????????????????????????????????????????????????????????", // 0
		 "?==============================================================================?", // 1
		 "?                                                                              ?", // 2
		 "?                                                                              ?", // 3
		 "?                                                                              ?", // 4
		 "?                                                                              ?", // 5
		 "?                          How many naz is this course?                        ?", // 6
		 "?                                                                              ?", // 7
		 "?                                                                              ?", // 9
		 "?                                                                              ?", // 9
		 "?                                                                              ?", // 10
		 "?                                                                              ?", // 11
		 "?                                                                              ?", // 12
		 "?                                                                              ?", // 13
		 "?                                                                              ?", // 14
		 "?                                Answer:                                       ?", // 15
		 "?                                                                              ?", // 16
		 "?                                                                              ?", // 17
		 "?                                                                              ?", // 18
		 "?                                                                              ?", // 19
		 "?                                                                              ?", // 20
		 "?                                                                              ?", // 21
		 "?                                                                              ?", // 22
		 "?==============================================================================?", // 23
		 "????????????????????????????????????????????????????????????????????????????????"  // 24
	};


	/*const char* endLoadBoard[MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "Q==============================================================================Q", // 1
		 "Q                                                                              Q", // 2
		 "Q                                                                              Q", // 3
		 "Q                                                                              Q", // 4
		 "Q                                                                              Q", // 5
		 "Q                    ||    ALL FILES HAVE BEEN LOADED    ||                    Q", // 6
		 "Q                                                                              Q", // 7
		 "Q                                                                              Q", // 9
		 "Q                                                                              Q", // 9
		 "Q                                                                              Q", // 10
		 "Q                                                                              Q", // 11
		 "Q                                                                              Q", // 12
		 "Q                                                                              Q", // 13
		 "Q                                                                              Q", // 14
		 "Q                                                                              Q", // 15
		 "Q                                                                              Q", // 16
		 "Q                                                                              Q", // 17
		 "Q                                                                              Q", // 18
		 "Q                                                                              Q", // 19
		 "Q                                                                              Q", // 20
		 "Q                                                                              Q", // 21
		 "Q                                                                              Q", // 22
		 "Q==============================================================================Q", // 23
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};*/

	const char* manuBoard[MAX_Y] = {
		// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
		  "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		  "W==============================================================================W", // 1
		  "W                                                                              W", // 2
		  "W                              ***   Manu   ***                                W", // 3
		  "W                                                                              W", // 4
		  "W                                                                              W", // 5
		  "W                                                                              W", // 6
		  "W                         Press 1 - Start a new game                           W", // 7
		  "W                                                                              W", // 8
		  "W                          Press 2 - Colors (ON/OFF)                           W", // 9
		  "W                                                                              W", // 10
		  "W                           Press 3 - Score Board                              W", // 11
		  "W                                                                              W", // 12
		  "W                                                                              W", // 13
		  "W                                                                              W", // 14
		  "W                                                                              W", // 15
		  "W                                                                              W", // 16
		  "W                   Press 8 - Present instructions and keys                    W", // 17
		  "W                                                                              W", // 18
		  "W                              Press 9 - EXIT                                  W", // 19
		  "W                                                                              W", // 20
		  "W                                                                              W", // 21
		  "W                                                                              W", // 22
		  "W==============================================================================W", // 23
		  "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};
	const char* guideBoard[MAX_Y] = {
		// 01234567890123456789012345678901234567890123456789012345678901234567890123456789
		  "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		  "W                                                                              W", // 1
		  "W                           ~~~ INSTRUCTIONS ~~~                               W", // 2
		  "W                                                                              W", // 3
		  "W                     PRESS ESC TO EXIT THE INSTRUCTIONS                       W", // 4
		  "W                         How to Play Adventure World:                         W", // 5
		  "W                                                                              W", // 6
		  "W    - The objective is to navigate your characters to the last room,          W", // 7
		  "W      by solving riddles and use different tools.                             W", // 8
		  "W    - Use the keys below to control your character.                           W", // 9
		  "W                                                                              W", // 10
		  "W                                CONTROLS:                                     W", // 11
		  "W            A - Move Left                     J - Move Left                   W", // 12
          "W            D - Move Right                    L - Move Right                  W", // 13
		  "W            W - Move Up                       I - Move Up                     W", // 14
		  "W            X - Move Down                     M - Move Down                   W", // 15
		  "W            S - Stay in Place                 K - Stay in Place               W", // 16
		  "W            E - Pick/Dispose Element          O - Pick/Dispose Element        W", // 17
		  "W                                                                              W", // 18
		  "W                                                                              W", // 19
		  "W                                                                              W", // 20
		  "W                                                                              W", // 21
		  "W                                                                              W", // 22
		  "W                          Good luck and have fun!                             W", // 23
		  "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};
	/*const char* noFilesErrorBoard[MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "Q==============================================================================Q", // 1
		 "Q                                                                              Q", // 2
		 "Q                                                                              Q", // 3
		 "Q                                                                              Q", // 4
		 "Q                                                                              Q", // 5
		 "Q                                                                              Q", // 6
		 "Q                      ||    Screen File is empty    ||                        Q", // 7
		 "Q                                                                              Q", // 9
		 "Q                         Add files in order to play                           Q", // 9
		 "Q                                                                              Q", // 10
		 "Q                                                                              Q", // 11
		 "Q                                                                              Q", // 12
		 "Q                                                                              Q", // 13
		 "Q                                                                              Q", // 14
		 "Q                                                                              Q", // 15
		 "Q                                                                              Q", // 16
		 "Q                                                                              Q", // 17
		 "Q                        Press ESC to return to the Menu                       Q", // 18
		 "Q                                                                              Q", // 19
		 "Q                                                                              Q", // 20
		 "Q                                                                              Q", // 21
		 "Q                                                                              Q", // 22
		 "Q==============================================================================Q", // 23
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};*/

	const char* gamePaused[MAX_Y] = {
		//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
			"W                                                                              W", // 1
			"W                                                                              W", // 2
			"W                                                                              W", // 3
			"W                                                                              W", // 4
			"W                                                                              W", // 5
			"W                                                                              W", // 6
			"W                                                                              W", // 7
			"W                                                                              W", // 8
			"W                                                                              W", // 9
			"W                          ||    GAME PAUSED    ||                             W", // 10
			"W                                                                              W", // 11
			"W                                                                              W", // 12
			"W                                                                              W", // 13
			"W                                                                              W", // 14
			"W                                                                              W", // 15
			"W                                                                              W", // 16
			"W                                                                              W", // 17
			"W                                                                              W", // 18
			"W                              PRESS ESC TO CONNOTE                            W", // 19
			"W                           PRESS H TO GO BACK TO MENU                         W", // 20
			"W                                                                              W", // 21
			"W                                                                              W", // 22
			"W                                                                              W", // 23
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};



public:
	Screen();
	~Screen();


	Screen& operator=(Screen const& other);

	Screen(const Screen& other);

	// color / cell helpers
	void setCellColor(const Point& p, Color c) { if (p.getX() >= 0 && p.getX() < MAX_X && p.getY() >= 0 && p.getY() < MAX_Y) cellColor[p.getY()][p.getX()] = c; }
	Color getCellColor(const Point& p) const { if (p.getX() >= 0 && p.getX() < MAX_X && p.getY() >= 0 && p.getY() < MAX_Y) return cellColor[p.getY()][p.getX()]; return Color::LightYellow; }
	int getCurrentRoomIndex() const { return currentRoomIndex; }

	// Per-room defaults
	void setRoomDefaultColor(int roomIndex, Color c);
	Color getRoomDefaultColor(int roomIndex) const;
	void setRoomUseColor(int roomIndex, bool use);
	bool isRoomUseColor(int roomIndex) const;

	// Save/restore state used for pause/unpause
	void saveStateForPause();
	void restoreStateFromPause();

	// choose screen
	//int chooseRoom(std::vector<std::string> const, const int sumOfFiles);
	void setScoreBoard();
	void setMenu();
	void setGuide();
	void setLose();
	void setWin();
	void setGamePaused();
	void setRiddle();
	void setRoom(int nRoom);
	
	

	void printBoard(Color col = Color::LightYellow) const;
	void printRoom(Color col = Color::Gray) const;


	// Searches for a specific character on the board
	Point searchChar(char ch) const;

	
	// Fix a sepcific char on the screen ( deletes the bad ones )
	void fixChar(char c);

	// Function to change the pixel at a specific position on the current board
	void changePixelInCurrBoard(const Point& p, char ch) {
		// Modify the character at the given position (x, y) in the current board
		currentBoard[p.getY()][p.getX()] = ch;
	}
	// Function to change the pixel at a specific position on the current screen
	void changePixelInRoom(const Point& p, char ch) {
		// Modify the character at the given position (x, y) in the current board
		currentRoom[p.getY()][p.getX()] = ch;
	}

	char getCharAtcurrentBoard(const Point& p) const {
		return currentBoard[p.getY()][p.getX()];
	}

	// Return the visible character at a room location. If a live Item occupies the tile
	// return its character; otherwise return the template char.
	char getCharAtcurrentRoom(const Point& p) const;


	void registerPlayers(Player* players, int count) { registeredPlayers = players; registeredPlayerCount = count; }
	Player* getRegisteredPlayers() { return registeredPlayers; }
	int getRegisteredPlayerCount() const { return registeredPlayerCount; }
	
	 // Hide any registered players whose positions are within `radius` of `center`.
	void hidePlayersInRadius(const Point& center, int radius);


	
	Item* getItem(const Point& p);
	void  addItem(Item* item) { items.push_back(item); }


	

	
	Item* peekItemAt(const Point& p) const;

	void gobacktoMenu();

	bool removeItemAt(const Point & p);

	bool isItem(const Point& p) const;

	bool isDoor(const Point& p) const;

	bool isWall(const Point& p) const {
		return getCharAtcurrentRoom(p) == 'W' || getCharAtcurrentRoom(p) == 'L';
	}

	int getVisiblePlayerCount() const;

	// Mark a door location as opened (persist for the current room)
	void markDoorOpened(const Point& p) { openedDoors.push_back(p); }

	// Query whether a door was opened at the given point (position equality)
	bool isDoorOpenedAt(const Point& p) const;

	// Clear opened-door registry (called when loading a new room)
	void clearOpenedDoors() { openedDoors.clear(); }

	// Called each game-loop tick to update armed bombs stored in `items`.
	void updateBombs();

		
	// Check whether the provided switch requirements are satisfied in the current live items.
    // Each requirement is (group == room number, requiredState).
	bool areSwitchRequirementsSatisfied(const vector<pair<int, bool>>& reqs) const;

	// Re-evaluate doors whose opening depends on switches. Called after switch toggles
	// and after room wiring to auto-open SwitchesOnly doors that are already satisfied.
	void evaluateDoorRequirements();
	void printPlayersinfo()const;
};

