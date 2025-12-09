#pragma once

#include <iostream>
#include "Point.h"
#include "CollectableItems.h"
#include "SteppedOnItems.h"
#include "Console.h"
#include "Door.h"
#include "Riddle.h"
#include <vector>


class Screen {
public:
	enum { MAX_X = 80, MAX_Y = 25 };
	char currentBoard[MAX_Y][MAX_X + 1] = {}; // +1 for null terminator
	char currentRoom[MAX_Y][MAX_X + 1] = {}; // +1 for null terminator
private:
	std::vector<Item*> items;
	std::vector<Point> openedDoors;

	const char* gameRoom1[MAX_Y] = {
		//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                          W    WWWW", // 1
			"WWWWW               WWWWWWWWW                                          W    WWWW", // 2
			"WWWWW                                                                  W    WWWW", // 3
			"WWWWW                                                                  W    WWWW", // 4
			"WWWWW                                                                  W    WWWW", // 5
			"WWWWW                                                                  W    WWWW", // 6
			"WWWWW                                                                       WWWW", // 7
			"WWWWW               WWWWWWWWW                                               WWWW", // 8
			"WWWWW               WWWWWWWWW                                               WWWW", // 9
			"WWWW                                                                          1 ", // 10
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                               WWWW", // 11
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                               WWWW", // 12
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                          W    WWWW", // 13
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                          W    WWWW", // 14
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW?WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 15
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWW    WWWW", // 16
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWW    WWWW", // 17
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWW    WWWW", // 18
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW          K          WWWWWWWWWWWWWWWW    WWWW", // 19
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWW    WWWW", // 20
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWW    WWWW", // 21
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW    WWWW", // 22
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW    WWWW", // 23
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};

	// tabs removed and layout normalized; lines may be shorter than MAX_X but Screen::setRoomX will pad safely.
	const char* gameRoom2[MAX_Y] = {
	//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		"WWWWWWWWWWWWW    WWWW                  WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 1
		"WWWWWWWWWWWWW    WWWWW                            WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 2
		"WWWWWWWWWWWWW    WWWWW  WW                        WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 3
		"WWWWWWWWWWWWW    WWWWW  WW            WW          WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 4
		"WWWWWWWWWWWWW    WWWWWWWWWWWW         WW          WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 5
		"WWWWWWWWWWWWW    WWWWWWWWWWWW        WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 6
		"W                   WWWW                            WWWW                       W", // 7
		"W                                                   WWWW                       W", // 8
		"W                                                   WWWW                       W", // 9
		"W         WW        WWWWWWWW                        WWWW                       W", // 10
		"W         WW        WWWWWWWW                        WWWW                       W", // 11
		"W         WW        WWWWWWWW                        WWWW                       W", // 12
		"W         WW        WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 13
		"W         WW        WWWWWWWW                        WWW              WWW       W", // 14
		"W         WW  WWWWWWWWWWWWW                         WWW              WWW       W", // 15
		"W         WWWWWWWWWWWWWWW                           WWW              W         W", // 16
		"W         WWWWWWWWWWWWWWW                           WWW              W         W", // 17
		"W                       W                           WWW              W         W", // 18
		"W                       W                           WWW              W         W", // 19
		"W                       W                           WWW              W         W", // 20
		"W              2        WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW         W", // 21
		"W                            WWW            WW              WW                 W", // 22
		"W                            WW        WW              WW           WW         W", // 23
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};

	
	const char* gameRoom3[MAX_Y] = {
	//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0 
		"W                               W                                              W", // 1
		"W  WWWWWWWWWWW  WWW      WWW   W   WWWWW   WWWWW   WWWW  W   WWWWWWWWWWWWWWW   W", // 2
		"W  W        W   W        W     W   W   W       W     W   W   W             W   W", // 3
		"W  W        W   WWWWWWW  W  WWWW   W   W  WWW  W  WWWW   W   W             W   W", // 4
		"W  WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW  W", // 5
		"W                                                                              W", // 6
		"W                                                                              W", // 7
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 8
		"W                                                                              W", // 9
		"W  W   W   W   W   W   W   W   W   W   W   W   W   W   W   W   W   W   W   W   W", // 10
		"W  W   W   W   W   W   W   W       W   W   W       W       W       W   W       W", // 11
		"W  W   WWWWW   WWWWW   WWWWW   WWWWW   WWW   WWWWW   WWWWW   WWWWW   WWWWW     W", // 12
		"W  W                                                                           W", // 13
		"W                                                                              W", // 14
		"W                                                                              W", // 15
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 16
		"W                                                                              W", // 17
		"W                                                                              W", // 18
		"W                                                                              W", // 19
		"W                                                                              W", // 20
		"W                                                                              W", // 21
		"W                                                                              W", // 22
		"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 23
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
			"W                              PRESS ESC TO CONINUE                            W", // 19
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

	

	// choose screen
	//int chooseRoom(std::vector<std::string> const, const int sumOfFiles);
	void setScoreBoard();
	void setMenu();
	void setGuide();
	void setLose();
	void setWin();
	void setScreenError();
	void setGamePaused();
	void setRiddle();


	void setRoom(int nRoom);
	//void setRoom2();
	//void setRoom3();
	// 
	// Sets the current board to the end load state
	//void setEndLoad();


	// Sets the current board to choose a screen state
	//void setChooseScreen();

	// Sets the current board to screen error state
	

	// Sets the current board to no files Error state
	//void setNoFilesError();

	// Sets and fix the current screen
	//bool setScreen(int i);

	// Checks if a screen is valid 
	//bool isScreenOk(int i);

	void printBoard() const;
	void printRoom() const;


	// Searches for a specific character on the board
	Point searchChar(char ch) const;

	// Fix the current screen so all the components are valid
	void fixBoard();

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
	char getCharAtcurrentRoom(const Point& p) const {
		// Check active items first
		for (Item* it : items) {
			if (!it) continue;
			Point ip = it->getPos();
			if (ip.getX() == p.getX() && ip.getY() == p.getY())
				return it->getCh();
		}
		// Fallback to template char
		return currentRoom[p.getY()][p.getX()];
	}


	// Returns a newly-allocated CollectableItems* if player picks up an item at `p`.
	// The item is removed from the activeCollectables in the loaded room.
	Item* getItem(const Point& p);
	void  addItem(Item* item) { items.push_back(item); }


	// Active items for the currently loaded room.
	// Stored by value to avoid needing virtual destructor on Item.


public:
	
	Item* peekItemAt(const Point& p) const;

	void gobacktoMenu();

	bool removeItemAt(const Point & p);

	bool isItem(const Point& p) const {
		// Check active items first
		for (Item* it : items) {
			if (!it) continue;
			Point ip = it->getPos();
			if (ip.getX() == p.getX() && ip.getY() == p.getY())
				return true;
		}
		// no item found
		return false;
	}

	bool isDoor(const Point& p) const {
		// Check active items for Door
		for (Item* it : items) {
			if (!it) continue;
			Door* d = dynamic_cast<Door*>(it);
			if (d) {
				Point ip = d->getPos();
				if (ip.getX() == p.getX() && ip.getY() == p.getY())
					return true;
			}
		}
		// Fallback to template char
		char c = currentRoom[p.getY()][p.getX()];
		return (c >= '1' && c <= '9');
	}

	bool isWall(const Point& p) const {
		return getCharAtcurrentRoom(p) == 'W';
	}

	// Mark a door location as opened (persist for the current room)
	void markDoorOpened(const Point& p) { openedDoors.push_back(p); }

	// Query whether a door was opened at the given point (position equality)
	bool isDoorOpenedAt(const Point& p) const {
		for (const Point& dp : openedDoors) {
			if (dp.getX() == p.getX() && dp.getY() == p.getY())
				return true;
		}
		return false;
	}

	// Clear opened-door registry (called when loading a new room)
	void clearOpenedDoors() { openedDoors.clear(); }
};

