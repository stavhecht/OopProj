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
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
            cellColor[y][x] = Color::Gray;
        }
    }

    // initialize per-room defaults and enable coloring per-room by default
    for (int i = 0; i < ROOM_COUNT; i++) {
        roomDefaultColor[i] = Color::Gray;
        roomUseColor[i] = false;
    }
    // set sensible defaults for the named rooms (1..3)
    roomDefaultColor[0] = Color::LightYellow; 
    roomDefaultColor[1] = Color::Blue;   
    roomDefaultColor[2] = Color::Black;        

    roomUseColor[0] = true;
    roomUseColor[1] = true;
    roomUseColor[2] = true;

    // Keep currentRoom empty until a room is selected (setRoom will populate it).
}

Screen::~Screen() {
    clearAndDeleteItems();
}

// --- private helpers ---

void Screen::clearAndDeleteItems() {
    for (Item* it : items) {
        delete it;
    }
    items.clear();
}

void Screen::copyTemplateToCurrentRoom(const char* const roomTemplate[]) {
    for (int i = 0; i < MAX_Y; ++i) {
        std::memcpy(currentRoom[i], roomTemplate[i], MAX_X + 1);
    }
}

void Screen::applyRoomDefaultColors(int nRoom) {
    // set per-cell color to the room default (or uncolored if roomUseColor is false)
    Color roomDefault = Color::Gray;

    if (nRoom >= 0 && nRoom < ROOM_COUNT) {
        // Special-case: always render room 3 (index 2) with its configured default (Black)
        // even when coloring is globally turned off for other rooms.
        if (nRoom == 2) {
            roomDefault = roomDefaultColor[nRoom];
        }
        else if (roomUseColor[nRoom]) {
            roomDefault = roomDefaultColor[nRoom];
        }
        else {
            roomDefault = Color::Gray;
        }
    }
    else {
        roomDefault = Color::Gray;
    }

    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
            cellColor[y][x] = roomDefault;
        }
    }
}

void Screen::populateLiveItemsFromRoom() {
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
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
                 changePixelInRoom(pos, ' ');
                 break;
             }
            default:
                break;
            }
        }
    }
}

// --- end private helpers ---


// --- per-room defaults ---
void Screen::setRoomDefaultColor(int roomIndex, Color c) {
    // now 0-based: valid indices 0..ROOM_COUNT-1
    if (roomIndex < 0 || roomIndex >= ROOM_COUNT) return;
    roomDefaultColor[roomIndex] = c;
}

Color Screen::getRoomDefaultColor(int roomIndex) const {
    // now 0-based: valid indices 0..ROOM_COUNT-1
    if (roomIndex < 0 || roomIndex >= ROOM_COUNT) return Color::Gray;
    return roomDefaultColor[roomIndex];
}

void Screen::setRoomUseColor(int roomIndex, bool use) {
    if (roomIndex < 0 || roomIndex >= ROOM_COUNT) return;
    roomUseColor[roomIndex] = use;
}

bool Screen::isRoomUseColor(int roomIndex) const {
    if (roomIndex < 0 || roomIndex >= ROOM_COUNT) return false;
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



void Screen::setGuide() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], guideBoard[i], MAX_X + 1); 
    }
}


void Screen::setWin() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], winBoard[i], MAX_X + 1);  
    }
}

void Screen::setLose() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], loseBoard[i], MAX_X + 1);  
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

// Copy ctor for Screen — performs deep copy of mutable buffers and pointer copy
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
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
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

    // Draw registered players 
    if (registeredPlayers && registeredPlayerCount > 0) {
        for (int i = 0; i < registeredPlayerCount; i++) {
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

    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
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
	currentRoomIndex = nRoom - 1;

    // Delete any existing live items before loading a new room
    clearAndDeleteItems();

    switch (currentRoomIndex)
    {
    case 0:
        copyTemplateToCurrentRoom(gameRoom1);
        break;
    case 1:
        copyTemplateToCurrentRoom(gameRoom2);
        break;
    case 2:
        copyTemplateToCurrentRoom(gameRoom3);
        break;
    default:
        break;
    }

    applyRoomDefaultColors(currentRoomIndex);

    
    populateLiveItemsFromRoom();
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


char Screen::getCharAtcurrentRoom(const Point& p) const {
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

bool Screen::isItem(const Point& p) const {
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

bool Screen::isDoor(const Point& p) const {
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
	// extra check: template char check for doors
    char c = currentRoom[p.getY()][p.getX()];
    return (c >= '1' && c <= '9');
}

bool Screen::isDoorOpenedAt(const Point& p) const {
    for (const Point& dp : openedDoors) {
        if (dp.getX() == p.getX() && dp.getY() == p.getY())
            return true;
    }
    return false;
}


void Screen::hidePlayersInRadius(const Point& center, int radius) {
    if (!registeredPlayers || registeredPlayerCount <= 0)
        return;

        int radiusSq = radius * radius;
    for (int i = 0; i < registeredPlayerCount; ++i) {
        Player & p = registeredPlayers[i];
        if (!p.isVisible())
            continue;

        Point pp = p.getPos();
        int dx = pp.getX() - center.getX();
        int dy = pp.getY() - center.getY();
        if (dx * dx + dy * dy <= radiusSq) {
            p.setVisible(false);
            p.setPos(Point(-1, -1));                
        }        
    }    
}

int Screen::getVisiblePlayerCount() const {
    if (!registeredPlayers || registeredPlayerCount <= 0) return 0;
    int cnt = 0;
    for (int i = 0; i < registeredPlayerCount; ++i) {
        if (registeredPlayers[i].isVisible()) ++cnt;
    }
    return cnt;
}



bool Screen::removeItemAt(const Point & p) {
    for (auto it = items.begin(); it != items.end(); it++) {
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

    
    for (auto it = items.begin(); it != items.end(); it++) {
        Item* item = *it;
        if (!item) continue;
        Point ip = item->getPos();
        if (ip.getX() == x && ip.getY() == y) {
            // Only CollectableItems can be picked up by player.
            CollectableItems* ci = dynamic_cast<CollectableItems*>(item);
            if (ci) {
                // transfer ownership: remove from screen list and return the pointer
                items.erase(it);
                changePixelInRoom(ip, ' ');
                return ci;
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
    clearAndDeleteItems();

    // copy mutable boards (char buffers)
    for (int i = 0; i < MAX_Y; i++) {
        std::memcpy(currentBoard[i], other.currentBoard[i], MAX_X + 1);
        std::memcpy(currentRoom[i], other.currentRoom[i], MAX_X + 1);
    }

    // copy pointer-based board templates
    for (int i = 0; i < MAX_Y; i++) {
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

    // deep copy items
    for (Item* it : other.items) {
        if (it) items.push_back(it->clone());
    }

    // copy color buffer
    for (int y = 0; y < MAX_Y; y++)
        for (int x = 0; x < MAX_X; x++)
            cellColor[y][x] = other.cellColor[y][x];

    hasSavedState = other.hasSavedState;
    if (hasSavedState) {
        for (int y = 0; y < MAX_Y; ++y) {
            std::memcpy(savedRoom[y], other.savedRoom[y], MAX_X + 1);
            for (int x = 0; x < MAX_X; x++)
                savedCellColor[y][x] = other.savedCellColor[y][x];
        }
    }

    return *this;
}