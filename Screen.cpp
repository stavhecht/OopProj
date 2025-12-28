#include "Screen.h"
#include "Console.h"
#include "Torch.h"
#include "Door.h"
#include "Riddle.h"
#include "Bomb.h"
#include <cstring>
#include <algorithm>
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
    roomDefaultColor[1] = Color::LightAqua;
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
        memcpy(currentRoom[i], roomTemplate[i], MAX_X + 1);
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

void Screen::populateLiveItemsFromRoom(int nRoom) {
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
                auto* spring = new Spring(pos, '#', Color::Gray);
                addItem(spring);
                changePixelInRoom(pos, ' ');
                break;
            }
            case '*': { // obstacle 
                auto* obstacle = new Obstacle(pos, '*', Color::Aqua);
                addItem(obstacle);
                changePixelInRoom(pos, ' ');
                break;
            }
            case '/': { // legacy ungrouped switch -> group = room number
                auto* switcher = new Switcher(pos, '/', Color::Red, nRoom);
                addItem(switcher);
                changePixelInRoom(pos, ' ');
                break;
            }
            default:
                // If you ever encode group using a digit/char, handle here.
                // Current policy: all '/' switches belong to the room they are placed in (group = nRoom).
                if (c >= '1' && c <= '9') {
                    auto* door = new Door(pos, c, Color::Purple);
                    addItem(door);
                    changePixelInRoom(pos, ' ');
                }
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
        memcpy(savedRoom[y], currentRoom[y], MAX_X + 1);
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
        memcpy(currentRoom[y], savedRoom[y], MAX_X + 1);
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
                for (int pi = 0; pi < registeredPlayerCount; ++pi) {
                    Player& p = registeredPlayers[pi];
                    if (!p.isVisible()) continue;
                    p.addScore(20);
                }
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

    // Render per-player scores starting at row 6 (fits inside board)
    if (registeredPlayers && registeredPlayerCount > 0) {
        const int startRow = 6;
        const int maxRows = MAX_Y - startRow - 3; // leave some footer space
        int rowsToShow = min(registeredPlayerCount, maxRows);
        for (int i = 0; i < rowsToShow; ++i) {
            string s = "Player " + to_string(i + 1) + " Score: " + to_string(registeredPlayers[i].getScore());
            writeTextToBoard(startRow + i, 10, s);
        }
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

void Screen::setErrorBoard(int nRoom) {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], screenErrorBoard[i], MAX_X + 1);
    }
	currentBoard[7][37] = '0' + nRoom; // write room number into the board
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

    // compute total score and write into the board near "Your Score:" (row 9)
    if (registeredPlayers && registeredPlayerCount > 0) {
        long long total = 0;
        for (int i = 0; i < registeredPlayerCount; ++i) total += registeredPlayers[i].getScore();
        string s = "Total Score: " + to_string(total);
        int col = (MAX_X - static_cast<int>(s.size())) / 2;
        if (col < 1) col = 1;
        writeTextToBoard(9, col, s);
    }
}

void Screen::setLose() {
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], loseBoard[i], MAX_X + 1);  
    }

    if (registeredPlayers && registeredPlayerCount > 0) {
        long long total = 0;
        for (int i = 0; i < registeredPlayerCount; ++i) total += registeredPlayers[i].getScore();
        string s = "Total Score: " + to_string(total);
        int col = (MAX_X - static_cast<int>(s.size())) / 2;
        if (col < 1) col = 1;
        writeTextToBoard(9, col, s);
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
    // copy basic scalars / pointers
    registeredPlayers = other.registeredPlayers;
    registeredPlayerCount = other.registeredPlayerCount;
    currentRoomIndex = other.currentRoomIndex;

    // copy mutable character buffers
    for (int i = 0; i < MAX_Y; ++i) {
        memcpy(currentBoard[i], other.currentBoard[i], MAX_X + 1);
        memcpy(currentRoom[i], other.currentRoom[i], MAX_X + 1);
    }

    // copy the pointer-based template boards (string literals) — shallow copy of pointers is correct
    for (int i = 0; i < MAX_Y; ++i) {
        winBoard[i] = other.winBoard[i];
        scoreBoard[i] = other.scoreBoard[i];
        screenErrorBoard[i] = other.screenErrorBoard[i];
        loseBoard[i] = other.loseBoard[i];

        manuBoard[i] = other.manuBoard[i];
        guideBoard[i] = other.guideBoard[i];
        gamePaused[i] = other.gamePaused[i];
    }

    // copy game room source data (text files -> vector<string>)
    gameRoomsData = other.gameRoomsData;

    // copy opened doors registry
    openedDoors = other.openedDoors;

    // copy per-room defaults arrays
    for (int i = 0; i < ROOM_COUNT; ++i) {
        roomDefaultColor[i] = other.roomDefaultColor[i];
        roomUseColor[i] = other.roomUseColor[i];
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
            memcpy(savedRoom[y], other.savedRoom[y], MAX_X + 1);
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

    for (int y = 0; y < MAX_Y - 1; y++) {
        cout << currentBoard[y] << '\n';
    }
    cout << currentBoard[MAX_Y - 1];
    reset_color();
}

void Screen::printRoom(Color defaultCol)const {
    // Move the cursor to the top-left corner
    system("cls");

    gotoxy(0, 0);

    // Print each character using the per-cell color buffer.
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
            Color c = cellColor[y][x];
            if (c == Color::Gray) {
                c = defaultCol;
            }
            set_color(c);
            cout << currentRoom[y][x];
        }
        if (y < MAX_Y - 1) cout << '\n';
    }
    reset_color();

    // Draw live items on top of the printed room.
    for (Item* it : items) {
        if (it) {
            it->draw();
        }
    }

    printPlayersinfo();
    // Draw registered players 
    if (registeredPlayers && registeredPlayerCount > 0) {
        for (int i = 0; i < registeredPlayerCount; i++) {
            if (registeredPlayers[i].isVisible())
                registeredPlayers[i].draw();
        }
    }

	reset_color();
}


// Screen.cpp - modified printPlayersinfo to display each player's numeric score after hearts.
// Replace the existing function body with this.
void Screen::printPlayersinfo() const {
    Point dis = searchChar('L'); // legend top-left marker
    if (dis.getX() < 0 || dis.getY() < 0 || !registeredPlayers || registeredPlayerCount <= 0) return;

    const int startX = dis.getX();
    const int startY = dis.getY() + 1;
    if (startX < 0 || startX >= MAX_X || startY < 0 || startY >= MAX_Y) return;

    const int gapSpaces = 5; // spaces between players
    int curCol = startX;

    for (int i = 0; i < registeredPlayerCount && curCol < MAX_X; ++i) {
        const Player& p = registeredPlayers[i];

        // position cursor before each player's block to avoid drift
        gotoxy(curCol, startY);

        string prefix = "P" + to_string(i + 1) + " lives = ";
        string hearts = build_hearts(p.getLifes());

        // Inventory char and color (fallback to '-' and White)
        char invCh = '-';
        Color invColor = Color::White;
        CollectableItems* inv = p.getInventory();
        if (inv) {
            invCh = inv->getCh();
            invColor = inv->getPos().getColor();
        }

        // Build block for width accounting (ASCII-only)
        string scoreStr = " score:" + to_string(p.getScore());
        string block = prefix + hearts + " inv " + string(1, invCh) + scoreStr;
        int printed = static_cast<int>(block.size());

        // Print prefix + hearts in player's color
        Color playerColor = p.getPos().getColor();
        set_color(playerColor);
        cout << prefix << hearts;

        // separator + "inv "
        cout << ' ' << "inv ";

        // print inventory char in its color (or '-' in white)
        set_color(invColor);
        cout << invCh;
        reset_color();

        // print score (use white or another neutral color)
        set_color(playerColor);
        cout << scoreStr;
        reset_color();

        // compute remaining space and print gap spaces without overflowing
        int remainingCols = MAX_X - (curCol + printed);
        int spacesToPrint = 0;
        if (remainingCols > 0) {
            spacesToPrint = min(gapSpaces, remainingCols);
            for (int s = 0; s < spacesToPrint; ++s) cout << ' ';
        }

        curCol += printed + spacesToPrint;
    }

    reset_color();
    cout.flush();
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
    currentRoomIndex = nRoom - 1;
    if (nRoom >= 1 && nRoom <= static_cast<int>(gameRoomsData.size())
        && !gameRoomsData[currentRoomIndex].empty()) {
        // copy lines into currentRoom (pad or truncate to MAX_X)
        const vector<string>& tmpl = gameRoomsData[currentRoomIndex];
        for (int r = 0; r < MAX_Y; ++r) {
            string line;
            if (r < static_cast<int>(tmpl.size()))
                line = tmpl[r];
            else
                line.clear();

            // ensure exact width MAX_X, null-terminated
            if (static_cast<int>(line.size()) < MAX_X)
                line.append(MAX_X - static_cast<int>(line.size()), ' ');
            else if (static_cast<int>(line.size()) > MAX_X)
                line.resize(MAX_X);

            // copy into char array with null terminator
            for (int c = 0; c < MAX_X; ++c)
                currentRoom[r][c] = line[c];
            currentRoom[r][MAX_X] = '\0';
        }
    }
    // clear opened doors for the new room
    clearOpenedDoors();
    currentRoomIndex = nRoom - 1;

    // Delete any existing live items before loading a new room
    clearAndDeleteItems();

    applyRoomDefaultColors(currentRoomIndex);

    // populate items and assign switches' group == room number
    populateLiveItemsFromRoom(nRoom);

    // Build wiring for this room only (door char -> mode and requirements)
    map<char, Door::OpenMode> doorModeMap;
    map<char, vector<pair<int, bool>>> doorReqMap;

    if (nRoom == 1) {
        // Room1: door '1' opens with key (legacy)
        doorModeMap['1'] = Door::OpenMode::KeyOnly;
    }
    else if (nRoom == 2) {
        // Room2: door '2' opens when all switches in room 2 are ON
        doorModeMap['2'] = Door::OpenMode::SwitchesOnly;
        doorReqMap['2'] = { { 2, true } }; // group = room number
    }
    else if (nRoom == 3) {
        // Room3: door '3' opens with key
        doorModeMap['3'] = Door::OpenMode::KeyOnly;
    }

    // Apply wiring: iterate live items and configure doors matching the maps.
    for (Item* it : items) {
        if (!it) continue;
        Door* d = dynamic_cast<Door*>(it);
        if (!d) continue;
        char digit = d->getCh(); // the char used when the door was created (e.g., '1','2',...)
        auto mIt = doorModeMap.find(digit);
        if (mIt != doorModeMap.end()) {
            d->setOpenMode(mIt->second);
        }
        auto rIt = doorReqMap.find(digit);
        if (rIt != doorReqMap.end()) {
            d->setSwitchRequirements(rIt->second);
        }
    }

    // After wiring, evaluate any doors that can auto-open immediately (SwitchesOnly).
    evaluateDoorRequirements();
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
    for (int i = 0; i < registeredPlayerCount; i++) {
        Player & p = registeredPlayers[i];
        if (!p.isVisible())
            continue;

        Point pp = p.getPos();
        int dx = pp.getX() - center.getX();
        int dy = pp.getY() - center.getY();
        if (dx * dx + dy * dy <= radiusSq) {
            p.setVisible(false);
			p.playerdead();
            p.setPos(Point(-1, -1));                
        }        
    }    
}

int Screen::getVisiblePlayerCount() const {
    if (!registeredPlayers || registeredPlayerCount <= 0) return 0;
    int cnt = 0;
    for (int i = 0; i < registeredPlayerCount; i++) {
        if (registeredPlayers[i].isVisible()) ++cnt;
    }
    return cnt;
}



bool Screen::removeItemAt(const Point& p) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        Item* item = *it;
        if (!item) continue;
        Point ip = item->getPos();
        if (ip.getX() == p.getX() && ip.getY() == p.getY()) {
            // Protect Doors that have been opened: do not remove them accidentally.
            Door* maybeDoor = dynamic_cast<Door*>(item);
            if (maybeDoor && maybeDoor->isOpen()) {
                // keep opened door in the items vector so it stays visible
                return false;
            }

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

    // copy basic scalars / pointers
    registeredPlayers = other.registeredPlayers;
    registeredPlayerCount = other.registeredPlayerCount;
    currentRoomIndex = other.currentRoomIndex;

    // copy mutable boards (char buffers)
    for (int i = 0; i < MAX_Y; i++) {
        memcpy(currentBoard[i], other.currentBoard[i], MAX_X + 1);
        memcpy(currentRoom[i], other.currentRoom[i], MAX_X + 1);
    }

    // copy pointer-based board templates
    for (int i = 0; i < MAX_Y; i++) {
        winBoard[i] = other.winBoard[i];
        scoreBoard[i] = other.scoreBoard[i];
        screenErrorBoard[i] = other.screenErrorBoard[i];
        loseBoard[i] = other.loseBoard[i];

        manuBoard[i] = other.manuBoard[i];
        guideBoard[i] = other.guideBoard[i];
        gamePaused[i] = other.gamePaused[i];
    }

    // copy game room source data
    gameRoomsData = other.gameRoomsData;

    // copy opened doors registry
    openedDoors = other.openedDoors;

    // copy per-room defaults arrays
    for (int i = 0; i < ROOM_COUNT; ++i) {
        roomDefaultColor[i] = other.roomDefaultColor[i];
        roomUseColor[i] = other.roomUseColor[i];
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
            memcpy(savedRoom[y], other.savedRoom[y], MAX_X + 1);
            for (int x = 0; x < MAX_X; x++)
                savedCellColor[y][x] = other.savedCellColor[y][x];
        }
    }

    return *this;
}



// Check whether the provided switch requirements are satisfied in the current live items.
bool Screen::areSwitchRequirementsSatisfied(const vector<pair<int,bool>>& reqs) const {
	if (reqs.empty()) return true;

	// For each required (groupId, state) ensure group exists and all switches in group match state.
	for (const auto& req : reqs) {
		char requiredGroup = req.first;
		bool requiredState = req.second;

		bool foundAnyInGroup = false;
		bool groupMatches = true;

		for (Item* it : items) {
			if (!it) continue;
			const Switcher* sw = dynamic_cast<Switcher*>(it);
			if (!sw) continue;
			if (sw->GetGroup() == requiredGroup) {
				foundAnyInGroup = true;
				if (sw->IsOn() != requiredState) {
					groupMatches = false;
					break;
				}
			}
		}

		// If group not present or any switch in group does not match required state -> fail
		if (!foundAnyInGroup || !groupMatches) {
			return false;
		}
	}

	return true;
}

// Open doors whose rules are purely switches-based and are now satisfied.
// This is called after a switch toggles to automatically open matching doors.
void Screen::evaluateDoorRequirements() {
	// Iterate a copy of items because we may remove items while iterating.
	vector<Item*> snapshot = items;

	for (Item* it : snapshot) {
		if (!it) continue;
		Door* door = dynamic_cast<Door*>(it);
		if (!door) continue;
		if (door->isOpen()) continue;

		// Only auto-open doors that are SwitchesOnly (not KeyOnly nor KeyAndSwitches)
		if (door->getOpenMode() != Door::OpenMode::SwitchesOnly)
			continue;

		const auto& reqs = door->getSwitchRequirements();
		if (areSwitchRequirementsSatisfied(reqs)) {
			// open programmatically (Door::open alone is not enough; persist to screen)
			door->open();

			Point dp = door->getPos();
			markDoorOpened(dp);
		}
	}
}



// Helper: safely overwrite a contiguous part of currentBoard[row] starting at col
void Screen::writeTextToBoard(int row, int col, const string& text) {
    if (row < 0 || row >= MAX_Y) return;
    if (col < 0) return;
    int maxWrite = min(static_cast<int>(text.size()), MAX_X - col);
    if (maxWrite <= 0) return;
    for (int i = 0; i < maxWrite; ++i) {
        currentBoard[row][col + i] = text[i];
    }
}

// Score helpers
void Screen::addScoreToPlayer(int playerIndex, int delta) {
    if (!registeredPlayers || playerIndex < 0 || playerIndex >= registeredPlayerCount) return;
    registeredPlayers[playerIndex].addScore(delta);
}

int Screen::getPlayerScore(int playerIndex) const {
    if (!registeredPlayers || playerIndex < 0 || playerIndex >= registeredPlayerCount) return 0;
    return registeredPlayers[playerIndex].getScore();
}

void Screen::resetAllScores() {
    if (!registeredPlayers) return;
    for (int i = 0; i < registeredPlayerCount; ++i) {
        registeredPlayers[i].resetScore();
    }
}

// update setScoreBoard to include per-player scores
