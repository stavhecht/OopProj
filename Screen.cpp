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

    // initialize cell colors to a sensible default for the whole buffer
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            cellColor[y][x] = Color::Gray;
        }
    }

    // initialize per-room defaults and enable coloring per-room by default
    for (int i = 0; i < ROOM_COUNT; ++i) {
        roomDefaultColor[i] = Color::Gray;
        roomUseColor[i] = false;
    }
    // set sensible defaults for the named rooms (1..3)
    roomDefaultColor[1] = Color::LightYellow; // room 1 ambient
    roomDefaultColor[2] = Color::Blue;   // room 2 ambient
    roomDefaultColor[3] = Color::Gray;        // room 3 ambient

    roomUseColor[1] = true;
    roomUseColor[2] = true;
    roomUseColor[3] = true;

    // Keep currentRoom empty until a room is selected (setRoom will populate it).
}

Screen::~Screen() {
    for (Item* it : items) {
        delete it;
    }
    items.clear();
}

// --- per-room defaults API implementations ---
void Screen::setRoomDefaultColor(int roomIndex, Color c) {
    if (roomIndex <= 0 || roomIndex >= ROOM_COUNT) return;
    roomDefaultColor[roomIndex] = c;
}

Color Screen::getRoomDefaultColor(int roomIndex) const {
    if (roomIndex <= 0 || roomIndex >= ROOM_COUNT) return Color::Gray;
    return roomDefaultColor[roomIndex];
}

void Screen::setRoomUseColor(int roomIndex, bool use) {
    if (roomIndex <= 0 || roomIndex >= ROOM_COUNT) return;
    roomUseColor[roomIndex] = use;
}

bool Screen::isRoomUseColor(int roomIndex) const {
    if (roomIndex <= 0 || roomIndex >= ROOM_COUNT) return false;
    return roomUseColor[roomIndex];
}

// Save/restore state used by pause
void Screen::saveStateForPause() {
    // save current room chars
    for (int y = 0; y < MAX_Y; ++y) {
        std::memcpy(savedRoom[y], currentRoom[y], MAX_X + 1);
        // save per-cell colors
        for (int x = 0; x < MAX_X; ++x) {
            savedCellColor[y][x] = cellColor[y][x];
        }
    }
    hasSavedState = true;
}

void Screen::restoreStateFromPause() {
    if (!hasSavedState) return;
    for (int y = 0; y < MAX_Y; ++y) {
        std::memcpy(currentRoom[y], savedRoom[y], MAX_X + 1);
        for (int x = 0; x < MAX_X; ++x) {
            cellColor[y][x] = savedCellColor[y][x];
        }
    }
    hasSavedState = false;
}

// Iterate live items and update any armed Bombs. Explode bombs whose fuse reaches zero.
void Screen::updateBombs() {
    // iterate by index so we can erase safely
    for (size_t i = 0; i < items.size(); ) {
        Item* it = items[i];
        Bomb* b = dynamic_cast<Bomb*>(it);
        if (b && b->isArmed()) {
            bool shouldExplode = b->tick();
            if (shouldExplode) {
                Point center = b->getPos();
                // remove and delete the Bomb item
                delete it;
                items.erase(items.begin() + i);
                // trigger explosion effect at captured center
                Bomb::triggerBomb(*this, center);
                // do not increment i (we removed current element)
                continue;
            }
        }
        i++;
    }
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

    // deep-copy items
    items.clear();
    for (Item* it : other.items) {
        if (it) items.push_back(it->clone());
    }

    // copy color buffer
    for (int y = 0; y < MAX_Y; ++y)
        for (int x = 0; x < MAX_X; ++x)
            cellColor[y][x] = other.cellColor[y][x];

    hasSavedState = other.hasSavedState;
    if (hasSavedState) {
        for (int y = 0; y < MAX_Y; ++y) {
            std::memcpy(savedRoom[y], other.savedRoom[y], MAX_X + 1);
            for (int x = 0; x < MAX_X; ++x)
                savedCellColor[y][x] = other.savedCellColor[y][x];
        }
    }
}

// Function to print the current board to the console (menu / simple boards)
void Screen::printBoard(Color col) const {
    // Move the cursor to the top-left corner
    system("cls");

    gotoxy(0, 0);

    // For boards (menu, paused, etc.) we keep previous behaviour: single color for whole board
    set_color(col);

    for (int y = 0; y < MAX_Y - 1; ++y) {
        std::cout << currentBoard[y] << '\n';
    }
    std::cout << currentBoard[MAX_Y - 1];
    reset_color();
}

void Screen::printRoom(Color defaultCol) const {
    // Move the cursor to the top-left corner
    system("cls");

    gotoxy(0, 0);

    // Print each character using the per-cell color buffer.
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            Color c = cellColor[y][x];
            set_color(c);
            std::cout << currentRoom[y][x];
        }
        if (y < MAX_Y - 1) std::cout << '\n';
    }
    reset_color();

    // Draw live items on top of the printed room.
    for (Item* it : items) {
        if (it) {
            it->draw();
        }
    }

    // Draw registered players (if any)
    if (registeredPlayers && registeredPlayerCount > 0) {
        for (int i = 0; i < registeredPlayerCount; ++i) {
            if (registeredPlayers[i].isVisible())
                registeredPlayers[i].draw();
        }
    }
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

void Screen::setRoom(int nRoom) {
    // clear opened doors for the new room
    clearOpenedDoors();
	currentRoomIndex = nRoom;

    // Delete any existing live items before loading a new room to avoid dangling pointers/leaks
    for (Item* it : items) {
        delete it;
    }
    items.clear();

    switch (nRoom)
    {
    case 1:
    {
        for (int i = 0; i < MAX_Y; i++)
            memcpy(currentRoom[i], gameRoom1[i], MAX_X + 1);  // Copy each row with an additional null terminator
        break;
    }
    case 2:
    {
        for (int i = 0; i < MAX_Y; i++)
            memcpy(currentRoom[i], gameRoom2[i], MAX_X + 1);
    }   break;
    case 3:
    {
        for (int i = 0; i < MAX_Y; i++)
            memcpy(currentRoom[i], gameRoom3[i], MAX_X + 1);
        break;
    }
    default:
        break;
    }

    // set per-cell color to the room default (or uncolored ambient if roomUseColor is false)
    Color roomDefault = Color::Gray;
    if (nRoom > 0 && nRoom < ROOM_COUNT && roomUseColor[nRoom]) {
        roomDefault = roomDefaultColor[nRoom];
    } else {
        // uncolored version -> choose neutral ambient (Gray)
        roomDefault = Color::Gray;
    }

    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            cellColor[y][x] = roomDefault;
        }
    }

    // populate live items from template (unchanged)
    for (int y = 0; y < MAX_Y; ++y) {
        for (int x = 0; x < MAX_X; ++x) {
            char c = currentRoom[y][x];
            Point pos(x, y);
            switch (c) {
            case '!': { // torch
                auto* torch = new Torch(pos, '!', Color::Yellow);
                addItem(torch);
                changePixelInRoom(pos, ' ');
                torch->draw();
                break;
            }
            case 'K': { // key
                auto* key = new CollectableItems(pos, 'K', Color::Green);
                addItem(key);
                changePixelInRoom(pos, ' ');
                break;
            }
            case '?': { // riddle tile
                auto* riddle = new Riddle(pos, '?', Color::LightAqua);
                addItem(riddle);
                changePixelInRoom(pos, ' ');
                break;
            }
            case '@': { // bomb
                auto* bomb = new Bomb(pos, '@', Color::Red);
                addItem(bomb);
                changePixelInRoom(pos, ' ');
                break;
            }
            case '#': { // spring
                auto* spring = new SteppedOnItems(pos, '#', Color::LightGreen);
                addItem(spring);
                changePixelInRoom(pos, ' ');
                break;
            }
            case '*': { // obstacle 
                auto* obstacle = new SteppedOnItems(pos, '*', Color::LightYellow);
                addItem(obstacle);
                changePixelInRoom(pos, ' ');
                break;
            }
            case '/': { // switcher
                auto* switcher = new SteppedOnItems(pos, '/', Color::LightPurple);
                addItem(switcher);
                changePixelInRoom(pos, ' ');
                break;
            }
            case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
                 auto* door = new Door(pos, c, Color::Red);
                 addItem(door);
                 // remove template char since door is now a live object
                 changePixelInRoom(pos, ' ');
                 break;
             }
            default:
                break;
            }
        }
    }
}

Item* Screen::peekItemAt(const Point& p) const {
    int x = p.getX();
    int y = p.getY();

    for (Item* it : items) {
        if (!it) continue;
        Point ip = it->getPos();
        if (ip.getX() == x && ip.getY() == y)
            return it;
    }
    return nullptr;
}


void Screen::hidePlayersInRadius(const Point& center, int radius) {
    if (!registeredPlayers || registeredPlayerCount <= 0)
        return;
    
        int radiusSq = radius * radius;
    for (int i = 0; i < registeredPlayerCount; ++i) {
        Player & p = registeredPlayers[i];
        Point pp = p.getPos();
        int dx = pp.getX() - center.getX();
        int dy = pp.getY() - center.getY();
        if (dx * dx + dy * dy <= radiusSq) {
            if (p.isVisible()) {
                p.setVisible(false);
                p.setPos(Point(-1, -1));
                
            }            
        }        
    }    
}



bool Screen::removeItemAt(const Point & p) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        Item * item = *it;
        if (!item) continue;
        Point ip = item->getPos();
        if (ip.getX() == p.getX() && ip.getY() == p.getY()) {
            delete item;
            items.erase(it);
            return true;
            
        }
        
    }
     return false;
}



Item* Screen::getItem(const Point& p) {
    int x = p.getX();
    int y = p.getY();

    // Find a collectable item at the given position in the active items list.
    for (auto it = items.begin(); it != items.end(); ++it) {
        Item* item = *it;
        if (!item) continue;
        Point ip = item->getPos();
        if (ip.getX() == x && ip.getY() == y) {
            // Only CollectableItems can be picked up by player.
            CollectableItems* ci = dynamic_cast<CollectableItems*>(item);
			SteppedOnItems* si = dynamic_cast<SteppedOnItems*>(item);
            if (ci) {
                // transfer ownership: remove from screen list and return the pointer
                items.erase(it);
                // ensure room template doesn't still contain the character
                changePixelInRoom(ip, ' ');
                return ci;
            }
            else if (si) {
                // stepped-on item, return pointer without removing from screen
				return si;
            }
            else {
                // not collectable
                return nullptr;
            }
        }
    }

    // fallback: if template still contains an item char, return nullptr (no live object)
    return nullptr;
}



Screen& Screen::operator=(Screen const& other) {
    if (this == &other) {
        return *this;
    }

    // delete existing items owned by this
    for (Item* it : items) {
        delete it;
    }
    items.clear();

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

    // deep-copy items
    for (Item* it : other.items) {
        if (it) items.push_back(it->clone());
    }

    // copy color buffer
    for (int y = 0; y < MAX_Y; ++y)
        for (int x = 0; x < MAX_X; ++x)
            cellColor[y][x] = other.cellColor[y][x];

    hasSavedState = other.hasSavedState;
    if (hasSavedState) {
        for (int y = 0; y < MAX_Y; ++y) {
            std::memcpy(savedRoom[y], other.savedRoom[y], MAX_X + 1);
            for (int x = 0; x < MAX_X; ++x)
                savedCellColor[y][x] = other.savedCellColor[y][x];
        }
    }

    return *this;
}