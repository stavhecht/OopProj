#include "Screen.h"
#include "Console.h"
#include "Torch.h"
#include "Door.h"
#include "Riddle.h"
#include "Bomb.h"


#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


// Initialize buffers so currentBoard/currentRoom are populated.
// Use setMenu() to populate currentBoard with the menu template on construction.
Screen::Screen() {
    setMenu();
	setRoom1();
    // Keep currentRoom empty until a room is selected (setRoomN will populate it).
}

void Screen::setScoreBoard() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], scoreBoard[i], MAX_X + 1);
    }
}

// Function to set the board to the menu state
void Screen::setMenu() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], manuBoard[i], MAX_X + 1);
    }
}

void Screen::setRiddle() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], riddleBoard[i], MAX_X + 1);
    }
}

void Screen::setGamePaused() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], gamePaused[i], MAX_X + 1);
    }
}


// Function to set the board to the screen state from a file
void Screen::fixBoard() {
    for (int i = 0; i < MAX_Y; i++) {
        strncpy_s(currentBoard[i], currentRoom[i], MAX_X + 1);
    }
}

// Function to set the board to the no files error state
/*void Screen::setNoFilesError() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], noFilesErrorBoard[i], MAX_X + 1);
    }
}*/

// Function to set the board to the guide state
void Screen::setGuide() {
    // Loop through all rows and copy the guide board to the current board
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], guideBoard[i], MAX_X + 1);  // Copy each row with an additional null terminator
    }
}

// Function to set the board to the end load state
/*void Screen::setEndLoad() {
    // Loop through all rows and copy the guide board to the current board
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], endLoadBoard[i], MAX_X + 1);  // Copy each row with an additional null terminator
    }
}*/


// Function to set the board to the Error screen state
void Screen::setScreenError() {
    // Loop through all rows and copy the guide board to the current board
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], screenErrorBoard[i], MAX_X + 1);  // Copy each row with an additional null terminator
    }
}

// Function to set the board to the win state
void Screen::setWin() {
    // Loop through all rows and copy the win board to the current board
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], winBoard[i], MAX_X + 1);  // Copy each row with an additional null terminator
    }
}

// Function to set the board to the lose state
void Screen::setLose() {
    // Loop through all rows and copy the lose board to the current board
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], loseBoard[i], MAX_X + 1);  // Copy each row with an additional null terminator
    }
}

void Screen::gobacktoMenu() {
    while (true) {
        if (check_kbhit()) {
            char key = static_cast<char>(get_single_char());
            if (key == 27) {
                setMenu();
                printBoard();
                return;
            }
        }

    }
}

// Copy constructor for Screen — performs deep copy of mutable buffers and pointer copy
Screen::Screen(const Screen& other) {
    // copy mutable character buffers
    for (int i = 0; i < MAX_Y; ++i) {
        std::memcpy(currentBoard[i], other.currentBoard[i], MAX_X + 1);
        std::memcpy(currentRoom[i], other.currentRoom[i], MAX_X + 1);
    }

    // copy the pointer-based template boards (string literals) — shallow copy of pointers is correct
    for (int i = 0; i < MAX_Y; ++i) {
        gameRoom1[i] = other.gameRoom1[i];
        gameRoom2[i] = other.gameRoom2[i];
        gameRoom3[i] = other.gameRoom3[i];

        winBoard[i] = other.winBoard[i];
        scoreBoard[i] = other.scoreBoard[i];
        screenErrorBoard[i] = other.screenErrorBoard[i];
        loseBoard[i] = other.loseBoard[i];

        manuBoard[i] = other.manuBoard[i];
        guideBoard[i] = other.guideBoard[i];
        gamePaused[i] = other.gamePaused[i];
    }
}

// Function to print the current board to the console
void Screen::printBoard() const {
    // Move the cursor to the top-left corner
    system("cls");

    gotoxy(0, 0);

    // Loop through all rows (except the last one) and print each line
    for (int i = 0; i < MAX_Y - 1; i++) {
        std::cout << currentBoard[i] << '\n';  // Print each row with a newline after it
    }
    // Print the last row without a newline after it
    std::cout << currentBoard[MAX_Y - 1];
}

void Screen::printRoom() const {
    // Move the cursor to the top-left corner
    system("cls");

    gotoxy(0, 0);
    set_color(Color::Yellow);

    // Loop through all rows (except the last one) and print each line
    for (int i = 0; i < MAX_Y - 1; i++) {
        std::cout << currentRoom[i] << '\n';  // Print each row with a newline after it
    }
    // Print the last row without a newline after it
    std::cout << currentRoom[MAX_Y - 1];
    reset_color();

}

// Function to search for a specific character on the board delete if there are duplicates
// Fix duplicates of character `c` in the currentRoom/currentBoard.
// Leaves the last occurrence and replaces earlier duplicates with a space.
void Screen::fixChar(char c) {
    bool found = false;
    Point res(-1, -1);

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            if (currentRoom[y][x] == c) {
                if (!found) {
                    res = Point(x, y);
                    found = true;
                } else {
                    // remove the previously recorded occurrence (replace with a single space)
                    int px = res.getX();
                    int py = res.getY();
                    if (px >= 0 && px < MAX_X && py >= 0 && py < MAX_Y) {
                        currentRoom[py][px] = ' ';
                        currentBoard[py][px] = ' ';
                    }
                    // record the new occurrence
                    res = Point(x, y);
                }
            }
        }
    }
}

// Function to search for a specific character on the board and return its position (if found)
Point Screen::searchChar(char c) const {
    // Loop through all rows and columns to find the character c
    Point res = { -1, -1 };
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
            if (currentRoom[y][x] == c) {
                res = { x, y };
            }
        }
    }
    return res;
}

void Screen::setRoom1() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentRoom[i], gameRoom1[i], MAX_X + 1);  // Copy each row with an additional null terminator
    }
    items.clear();
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            char c = currentRoom[y][x];
            Point pos(x, y);
            switch (c) {
            case '!': { // torch
                auto* torch = new Torch(pos, '!', Color::Yellow);
                addItem(torch);
                break;
            }
            case 'K': { // key
                auto* key = new CollectableItems(pos, 'K', Color::Green);
                addItem(key);
                break;
            }
            case '?': { // riddle tile
                auto* riddle = new Riddle(pos, '?', Color::LightAqua);
                addItem(riddle);
                break;
            }
            case '@': { // bomb
                auto* bomb = new Bomb(pos, '@', Color::Red);
                addItem(bomb);
                break;
            }
            case '#': { // spring
                auto* spring = new SteppedOnItems(pos, '#', Color::LightGreen);
                addItem(spring);
                break;
            }
            case '*': { // obstacle 
                auto* obstacle = new SteppedOnItems(pos, '*', Color::LightYellow);
                addItem(obstacle);
                break;
            }
            case '/': { // switcher
                auto* switcher = new SteppedOnItems(pos, '/', Color::LightPurple);
                addItem(switcher);
                break;
            }
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                auto* door = new Door(pos, c, Color::Red);
                addItem(door);
                break;
            }
            default:
                break;
            }
        }
    }
}



void Screen::setRoom2() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentRoom[i], gameRoom2[i], MAX_X + 1);  // Copy each row with an additional null terminator
    }
    items.clear();
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            char c = currentRoom[y][x];
            Point pos(x, y);
            switch (c) {
            case '!': { // torch
                auto* torch = new Torch(pos, '!', Color::Yellow);
                addItem(torch);
                break;
            }
            case 'K': { // key
                auto* key = new CollectableItems(pos, 'K', Color::Green);
                addItem(key);
                break;
            }
            case '?': { // riddle tile
                auto* riddle = new Riddle(pos, '?', Color::LightAqua);
                addItem(riddle);
                break;
            }
            case '@': { // bomb
                auto* bomb = new Bomb(pos, '@', Color::Red);
                addItem(bomb);
                break;
            }
            case '#': { // spring
                auto* spring = new SteppedOnItems(pos, '#', Color::LightGreen);
                addItem(spring);
                break;
            }
            case '*': { // obstacle 
                auto* obstacle = new SteppedOnItems(pos, '*', Color::LightYellow);
                addItem(obstacle);
                break;
            }
            case '/': { // switcher
                auto* switcher = new SteppedOnItems(pos, '/', Color::LightPurple);
                addItem(switcher);
                break;
            }
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                auto* door = new Door(pos, c, Color::Red);
                addItem(door);
                break;
            }
            default:
                break;
            }
        }
    }
}

void Screen::setRoom3() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentRoom[i], gameRoom3[i], MAX_X + 1);  // Copy each row with an additional null terminator
    }
    items.clear();
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            char c = currentRoom[y][x];
            Point pos(x, y);
            switch (c) {
            case '!': { // torch
                auto* torch = new Torch(pos, '!', Color::Yellow);
                addItem(torch);
                break;
            }
            case 'K': { // key
                auto* key = new CollectableItems(pos, 'K', Color::Green);
                addItem(key);
                break;
            }
            case '?': { // riddle tile
                auto* riddle = new Riddle(pos, '?', Color::LightAqua);
                addItem(riddle);
                break;
            }
            case '@': { // bomb
                auto* bomb = new CollectableItems(pos, '@', Color::Red);
                addItem(bomb);
                break;
            }
            case '#': { // spring
                auto* spring = new SteppedOnItems(pos, '#', Color::LightGreen);
                addItem(spring);
                break;
            }
            case '*': { // obstacle 
                auto* obstacle = new SteppedOnItems(pos, '*', Color::LightYellow);
                addItem(obstacle);
                break;
            }
            case '/': { // switcher
                auto* switcher = new SteppedOnItems(pos, '/', Color::LightPurple);
                addItem(switcher);
                break;
            }
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                auto* door = new Door(pos, c, Color::Red);
                addItem(door);
                break;
            }
            default:
                break;
            }
        }
    }
}



Item* Screen::getItem(const Point& p) {
    int x = p.getX();
    int y = p.getY();

    // First, check there is an item char at that position at all
    char c = getCharFromOriginalRoom(p);
    if (!isItem(p)) {
        return nullptr;
    }

    // Then, find the matching Item object by position
    for (Item* it : items) {
        if (!it) continue;
        Point ip = it->getPos();
        if (ip.getX() == x && ip.getY() == y) {
            return it;
        }
    }

    return nullptr;
}




Screen& Screen::operator=(Screen const& other) {
    if (this == &other) {
        return *this;
    }

    // copy mutable boards (char buffers)
    for (int i = 0; i < MAX_Y; ++i) {
        std::memcpy(currentBoard[i], other.currentBoard[i], MAX_X + 1);
        std::memcpy(currentRoom[i], other.currentRoom[i], MAX_X + 1);
    }

    // copy pointer-based board templates (shallow copy of pointers is correct here)
    for (int i = 0; i < MAX_Y; ++i) {
        gameRoom1[i] = other.gameRoom1[i];
        gameRoom2[i] = other.gameRoom2[i];
        gameRoom3[i] = other.gameRoom3[i];

        winBoard[i] = other.winBoard[i];
        scoreBoard[i] = other.scoreBoard[i];
        screenErrorBoard[i] = other.screenErrorBoard[i];
        loseBoard[i] = other.loseBoard[i];

        manuBoard[i] = other.manuBoard[i];
        guideBoard[i] = other.guideBoard[i];
        gamePaused[i] = other.gamePaused[i];
    }

    return *this;
}