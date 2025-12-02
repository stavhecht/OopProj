#pragma once

#include <iostream>
#include "Point.h"
#include <vector>


class Screen {
public:
	enum { MAX_X = 80, MAX_Y = 25 };
	char currentBoard[MAX_Y][MAX_X + 1]; // +1 for null terminator
	char currentRoom[MAX_Y][MAX_X + 1]; // +1 for null terminator
private:
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
			"                                                                                ", // 10
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                               WWWW", // 11
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                               WWWW", // 12
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                          W    WWWW", // 13
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWW                                          W    WWWW", // 14
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 15
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWW    WWWW", // 16
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWW    WWWW", // 17
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWW    WWWW", // 18
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWW    WWWW", // 19
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWW    WWWW", // 20
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW                     WWWWWWWWWWWWWWWW    WWWW", // 21
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW    WWWW", // 22
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW    WWWW", // 23
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};

	const char* gameRoom2[MAX_Y] = {
		//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
			"WWWWWWWWWWWWW	   WWWW					WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 1
			"WWWWWWWWWWWWW	   WWWWW							WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 2
			"WWWWWWWWWWWWW	   WWWWW	 WW						WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 3
			"WWWWWWWWWWWWW	   WWWWW	 WW		    WW			WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 4
			"WWWWWWWWWWWWW	   WWWWWWWWWWWW         WW			WWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 5
			"WWWWWWWWWWWWW	   WWWWWWWWWWWW       WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 6
			"W				   WWWW							WWWW							W", // 7
			"W												WWWW							W", // 8
			"W												WWWW							W", // 9
			"W       WW     WWWWWWWW                        WWWW                            W", // 10
			"W	     WW		WWWWWWWW						WWWW							W", // 11
			"W	     WW		WWWWWWWW						WWWW							W", // 12
			"W		 WW		WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW			W", // 13
			"W		 WW		WWWWWWWW											WWW			W", // 14
			"W		 WW	WWWWWWWWWWWW											WWW			W", // 15
			"W		 WWWWWWWWWWWWWWW											WWW	        W", // 16
			"W		 WWWWWWWWWWWWWWW											WWW         W", // 17
			"W					   W											WWW	        W", // 18
			"W					   W 											WWW	        W", // 19
			"W					   W											WWW	        W", // 20
			"W					   WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW		    W", // 21
			"W							WWW			WW				WW			 WW	        W", // 22
			"W                          WWW         WW              WW           WW         W", // 23
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};



	const char* gameRoom3[MAX_Y] = {
		//   01234567890123456789012345678901234567890123456789012345678901234567890123456789
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
			"W																				W", // 1
			"W																				W", // 2
			"W																				W", // 3
			"W																				W", // 4
			"W																				W", // 5
			"W																				W", // 6
			"W																				W", // 7
			"W																				W", // 8
			"W																				W", // 9
			"W                                                                              W", // 10
			"W																				W", // 11
			"W																				W", // 12
			"W																				W", // 13
			"W																				W", // 14
			"W																				W", // 15
			"W																		        W", // 16
			"W																		        W", // 17
			"W																		        W", // 18
			"W																		        W", // 19
			"W																		        W", // 20
			"W																		        W", // 21
			"W																		        W", // 22
			"W                                                                              W", // 23
			"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"  // 24
	};



	const char* winBoard[MAX_Y] = {
		//01234567890123456789012345678901234567890123456789012345678901234567890123456789
		 "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", // 0
		 "W==============================================================================W", // 1
		 "W                                                                              W", // 2
		 "W                                                                              W", // 3
		 "W                                                                              W", // 4
		 "W                                                                              W", // 5
		 "W                   ||    You Completed all the stages    ||                   W", // 6
		 "W                                                                              W", // 7
		 "W                                                                              W", // 9
		 "W                                Your Score:                                  W", // 9
		 "W                                                                              W", // 10
		 "W                                                                              W", // 11
		 "W                                                                              W", // 12
		 "W                                                                              W", // 13
		 "W                                                                              W", // 14
		 "W                                                                              W", // 15
		 "W                                                                             W", // 16
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
	Screen() {};
	// Assignment operator
	Screen& operator=(Screen const& other);

	Screen(const Screen& other);

	// Sets the current board to the score board state
	void setScoreBoard();

	// choose screen
	//int chooseRoom(std::vector<std::string> const, const int sumOfFiles);

	// Sets the current board to the menu state
	void setMenu();

	// Sets the current board to the guide state
	void setGuide();

	// Sets the current board to the lose state
	void setLose();

	// Sets the current board to the end load state
	//void setEndLoad();

	// Sets the current board to the win state
	void setWin();

	// Sets the current board to choose a screen state
	//void setChooseScreen();

	// Sets the current board to screen error state
	void setScreenError();

	void setGamePaused();

	// Sets the current board to no files Error state
	//void setNoFilesError();

	// Sets and fix the current screen
	//bool setScreen(int i);

	// Checks if a screen is valid 
	//bool isScreenOk(int i);

	// Prints the current board to the console
	void print() const;

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

	char getCharFromOriginalRoom(const Point& p) const {
		return currentRoom[p.getY()][p.getX()];
	}
	void setRoom1() const;

	void gobacktoMenu();

	bool isWall(const Point& p) const {
		return getCharFromOriginalRoom(p) == 'W';
	}
};

