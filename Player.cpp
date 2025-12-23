#include "Player.h"
#include "Bomb.h"
#include "Torch.h"
#include "Obstacle.h"
#include "Spring.h"
#include <cctype>
#include <vector>
#include <functional>
#include <cstdlib> // for std::abs

// --- Small helpers extracted to reduce duplication and clarify intent ---
// These are static member definitions (qualified) so they can access private members if needed.

// Return the point that would result if `p` stepped one cell in its stored direction
Point Player::nextPointFor(const Player& p) {
    Point np = p.getPos();
    np.move();
    return np;
}

 bool Player::hasDied() {
    if (lifes < lastKnownlifes) {
        lastKnownlifes = lifes;
        return true;
        
    }
        // update stored value in case life increased or unchanged
    lastKnownlifes = lifes;
    return false;
    
}

// Return the point that results from stepping `pt` one cell in `dir`
Point Player::stepPointFrom(const Point& pt, Direction dir) {
    Point np = pt;
    switch (dir) {
    case Direction::UP:    np = Point(pt.getX(), pt.getY() - 1, pt.getCh(), pt.getColor()); break;
    case Direction::RIGHT: np = Point(pt.getX() + 1, pt.getY(), pt.getCh(), pt.getColor()); break;
    case Direction::DOWN:  np = Point(pt.getX(), pt.getY() + 1, pt.getCh(), pt.getColor()); break;
    case Direction::LEFT:  np = Point(pt.getX() - 1, pt.getY(), pt.getCh(), pt.getColor()); break;
    default: break;
    }
    return np;
}

// Check whether `other` is adjacent to `target` and is attempting to push into `target`.
// We consider "attempting to push" when other.getPos().move() would equal the target (i.e. they
// have a non-STAY direction toward the target). Using a copy of the Point avoids side-effects.
bool Player::otherIsPushingTowards(const Player& other, const Point& target) {
    if (!other.isVisible()) return false;
    const Point otherPos = other.getPos();
    int mdx = abs(otherPos.getX() - target.getX());
    int mdy = abs(otherPos.getY() - target.getY());
    if (mdx + mdy != 1) return false; // must be orthogonally adjacent

    Point oNext = otherPos;
    oNext.move(); // uses the direction stored in otherPos copy
    return (oNext == target);
}

// Aggregate combined force from `self` and any cooperating registered players that are
// adjacent to `obstacleCell` and are attempting to push into it.
int Player::computeCombinedForceForPush(Screen& screen, const Player& self, const Point& obstacleCell) {
    int combined = self.currentForce();

    Player* reg = screen.getRegisteredPlayers();
    int regCount = screen.getRegisteredPlayerCount();
    if (!reg || regCount <= 0) return combined;

    for (int i = 0; i < regCount; ++i) {
        const Player& other = reg[i];
        if (&other == &self) continue;
        if (!other.isVisible()) continue;

        if (otherIsPushingTowards(other, obstacleCell)) {
            combined += other.currentForce();
        }
    }
    return combined;
}

// Try to perform the push (check canPushGroup + pushGroup) and, on success, update player's
// position, torch and visuals. Returns true if push succeeded and player moved into `next`.
// launchRemainingRef is nullable: caller may pass nullptr when not relevant.
bool Player::tryPerformPush(Screen& screen, Player& player, const Point& next, Direction pushDir, int combinedForce, int launchSpeedIfAny, int* launchRemainingRef) {
    if (!Obstacle::canPushGroup(screen, next, pushDir, combinedForce))
        return false;

    // Perform push
    if (!Obstacle::pushGroup(screen, next, pushDir))
        return false;

    // move player into freed cell and update visuals
    player.updateTorchOnMove(next);
    player.pos.draw(' ');
    player.pos = next;
    player.pos.draw();

    // full redraw to ensure visuals consistent after group push
    screen.printRoom();

    // Caller owns launchRemaining; if they passed a pointer they can modify it if desired.
    (void)launchSpeedIfAny;
    (void)launchRemainingRef;
    return true;
}

// End of helpers
// ----------------

Player::Player(const Point& point, const char(&the_keys)[NUM_KEYS + 1], Screen& room)
    : pos(point), screen(room) {
    for (int i = 0; i < NUM_KEYS; ++i) {
        keys[i] = the_keys[i];
    }
	lastKnownlifes = lifes;
}

Player& Player::operator=(const Player& other) {
    if (this != &other) {
        // Do not reassign the screen reference (references cannot be reseated).
        // If copying between different screens is needed, that must be handled explicitly elsewhere.
        pos = other.pos;
        for (int i = 0; i < NUM_KEYS; ++i) {
            keys[i] = other.keys[i];
        }
        // screen = other.screen; // intentionally omitted
        inventory = other.inventory;
        launchSpeed = other.launchSpeed;
        launchRemaining = other.launchRemaining;
        launchDir = other.launchDir;
        visible = other.visible;
    }
    return *this;
}

void Player::setPos(const Point& p) {
    // keep current glyph/color, but update x/y (and keep diff_x/diff_y)
    Point newPos(p.getX(), p.getY(), pos.getCh(), pos.getColor());
    pos = newPos;
}


void Player::draw() {
    if (!visible) return;
    pos.draw();
}

void Player::applyLaunch(int speed, Direction dir, int duration) {
    if (speed <= 0) return;
    launchSpeed = speed;
    switch (dir){
    case Direction::UP:
        launchDir = Direction::DOWN;
        break;
    case Direction::DOWN:
        launchDir = Direction::UP;
        break;
    case Direction::LEFT:
        launchDir = Direction::RIGHT;
        break;
    case Direction::RIGHT:
        launchDir = Direction::LEFT;
        break;
    default:
        break;
    }
    launchRemaining = duration;
}

void Player::pickUp() {
    if (!visible) return;

    auto res = pos.ItemInRadios(screen, 1);
    bool found = res.first;
    Point itemPos = res.second;

    if (!found || inventory != nullptr)
        return;

    // get item from screen (removes it from screen.items)
    CollectableItems* item = dynamic_cast<CollectableItems*>(screen.getItem(itemPos));
    if (!item)
        return;

    inventory = item;

    // If the picked up item is a Torch, move the light from the ground to the player
    if (Torch* t = dynamic_cast<Torch*>(inventory)) {
        // restore cells previously lit by the torch on ground and paint around player
        t->paintLightDiff(screen, itemPos, pos);
    }
    addScore(10);
    // remove from room (getItem already removed, this is defensive)
    screen.changePixelInRoom(itemPos, ' ');

    // item reaction
    inventory->onPickUp(*this, screen);

    // Redraw the room
    screen.printRoom();
}

CollectableItems* Player::dispose() {
    if (!visible) return nullptr;

    if (!inventory)
        return nullptr;

    auto res = pos.PlaceToDrop(screen, 1);
    bool found = res.first;
    if (!found)
        return nullptr;

    Point dropPos = res.second;

    // set drop position from inventory (uses operator Point conversion)
    dropPos = *inventory;
    inventory->setPos(dropPos);

    // item reaction
    inventory->onDrop(*this, screen);
    addScore(10);
    // If it's a bomb, arm it and transfer ownership to the screen
    Bomb* bomb = dynamic_cast<Bomb*>(inventory);
    if (bomb) {
        // transfer ownership to screen and arm fuse
        screen.addItem(bomb);
        constexpr int fuseTicks = 5; // explodes after 5 game-loop ticks
        bomb->arm(fuseTicks);
        screen.printRoom();
        inventory = nullptr;
        return nullptr;
    }

    // If it's a torch, transfer it to the screen and make it a persistent light source
    Torch* torch = dynamic_cast<Torch*>(inventory);
    if (torch) {
        // move light from player's position to the dropped position (restores old cells)
        torch->paintLightDiff(screen, pos, dropPos);

        // now transfer ownership to screen as a live item
        screen.addItem(torch);

        // final redraw
        screen.printRoom();
        inventory = nullptr;
        return nullptr;
    }

    // Non bomb, normal drop behavior add to screen as live item
    screen.addItem(inventory);
    screen.printRoom();
    CollectableItems* tmp = inventory;
    inventory = nullptr;
    return tmp;
}

CollectableItems* Player::takeInventory() {
    CollectableItems* tmp = inventory;
    inventory = nullptr;
    return tmp;
}

void Player::setVisible(bool v) {
    if (visible == v) return;

    if (!v) {
        // erase current player sign from console before hiding
        pos.draw(' ');
    } else {
        // if becoming visible again, draw at current pos
        pos.draw();
    }
    visible = v;
}

static Point stepPointDirLocal(const Point& p, Direction dir) {
    switch (dir) {
    case Direction::UP:    return Point(p.getX(), p.getY() - 1, p.getCh(), p.getColor());
    case Direction::RIGHT: return Point(p.getX() + 1, p.getY(), p.getCh(), p.getColor());
    case Direction::DOWN:  return Point(p.getX(), p.getY() + 1, p.getCh(), p.getColor());
    case Direction::LEFT:  return Point(p.getX() - 1, p.getY(), p.getCh(), p.getColor());
    default: return p;
    }
}

void Player::move() {
    if (!visible) return;

    // --- Launched movement (spring) ---
    if (launchRemaining > 0 && launchSpeed > 0) {
        int steps = launchSpeed;
        for (int s = 0; s < steps; ++s) {
            Point next = pos;
            next = stepPointDirLocal(next, launchDir);

            // blocked by wall -> stop launch
            if (screen.isWall(next)) {
                launchRemaining = 0;
                launchSpeed = 0;
                launchDir = Direction::STAY;
                pos.draw();
                return;
            }

            // If next contains a non-obstacle item -> stop launch (but attempt transfer to player collision)
            if (screen.isItem(next) && !dynamic_cast<Obstacle*>(screen.peekItemAt(next))) {
                // transfer launch if collided with other player
                Player* reg = screen.getRegisteredPlayers();
                int regCount = screen.getRegisteredPlayerCount();
                if (reg && regCount > 0) {
                    for (int i = 0; i < regCount; ++i) {
                        Player& other = reg[i];
                        if (&other == this) continue;
                        if (!other.isVisible()) continue;
                        if (other.getPos() == next) {
                            other.applyLaunch(launchSpeed, launchDir, launchSpeed * launchSpeed);
                        }
                    }
                }
                launchRemaining = 0;
                launchSpeed = 0;
                launchDir = Direction::STAY;
                pos.draw();
                return;
            }

            // If obstacle encountered, attempt to push (launched player should be able to push)
            Item* it = screen.peekItemAt(next);
            Obstacle* obs = dynamic_cast<Obstacle*>(it);
            if (obs) {
                Direction pushDir = launchDir;

                // Compute combined force using snapshot logic in helper
                int combinedForce = computeCombinedForceForPush(screen, *this, next);

                if (Obstacle::canPushGroup(screen, next, pushDir, combinedForce)) {
                    // perform push and move into freed cell
                    if (tryPerformPush(screen, *this, next, pushDir, combinedForce, launchSpeed, &launchRemaining)) {

                        // after moving into the cell, check collision-transfer of launch to other player
                        Player* reg2 = screen.getRegisteredPlayers();
                        int regCount2 = screen.getRegisteredPlayerCount();
                        if (reg2 && regCount2 > 0) {
                            for (int i = 0; i < regCount2; i++) {
                                Player& other = reg2[i];
                                if (&other == this) continue;
                                if (!other.isVisible()) continue;
                                if (other.getPos() == pos) {
                                    other.applyLaunch(launchSpeed, launchDir, launchSpeed * launchSpeed);
									other.addScore(5);
                                }
                            }
                        }

                        // consume one step of launch and continue to next step
                        if (launchRemaining > 0) --launchRemaining;
                        if (launchRemaining == 0) {
                            launchSpeed = 0;
                            launchDir = Direction::STAY;
                            break;
                        }
                        continue; // continue launch loop
                    }
                }

                // cannot push -> stop launch and stay
                launchRemaining = 0;
                launchSpeed = 0;
                launchDir = Direction::STAY;
                pos.draw();
                return;
            }

            // allowed: move one step (no obstacle)
            updateTorchOnMove(next);
            pos.draw(' ');
            pos = next;
            pos.draw();

            // check collision transfer launch to other player at new pos
            Player* reg = screen.getRegisteredPlayers();
            int regCount = screen.getRegisteredPlayerCount();
            if (reg && regCount > 0) {
                for (int i = 0; i < regCount; ++i) {
                    Player& other = reg[i];
                    if (&other == this) continue;
                    if (!other.isVisible()) continue;
                    if (other.getPos() == pos) {
                        other.applyLaunch(launchSpeed, launchDir, launchSpeed * launchSpeed);
                    }
                }
            }

            if (launchRemaining > 0) --launchRemaining;
            if (launchRemaining == 0) {
                launchSpeed = 0;
                launchDir = Direction::STAY;
                break;
            }
        }
        return;
    }

    // --- Normal (non-launched) movement ---
    Point next = pos;
    next.move();

    // If player is not attempting to move (STAY), just redraw and exit — do NOT push obstacles.
    if (next == pos) {
        pos.draw();
        return;
    }

    // Block on walls
    if (screen.isWall(next)) {
        pos.draw();
        return;
    }

    // If next contains an item, only allow push when it's an Obstacle and the combined force suffices.
    Item* it = screen.peekItemAt(next);
    Obstacle* obs = dynamic_cast<Obstacle*>(it);
    if (it && !obs) {
        // some other item (door, collectable, etc.) blocks movement here; keep position
        pos.draw();
        return;
    }

    if (obs) {
        // Determine push direction (from pos -> next)
        Direction pushDir = Direction::STAY;
        if (next.getX() - pos.getX() == 1) pushDir = Direction::RIGHT;
        else if (next.getX() - pos.getX() == -1) pushDir = Direction::LEFT;
        else if (next.getY() - pos.getY() == 1) pushDir = Direction::DOWN;
        else if (next.getY() - pos.getY() == -1) pushDir = Direction::UP;

        // Compute combined force using helper (consistent snapshot logic)
        int combinedForce = computeCombinedForceForPush(screen, *this, next);

        if (Obstacle::canPushGroup(screen, next, pushDir, combinedForce)) {
            // perform push and then move player into freed cell
            if (tryPerformPush(screen, *this, next, pushDir, combinedForce, 0, nullptr)) {
                // NOTE: the launch-related outputs are unused in non-launched path
                return;
            }
        }

        // cannot push -> stay in place
        pos.draw();
        return;
    }

    // Empty cell -> move normally
    updateTorchOnMove(next);
    pos.draw(' ');
    pos = next;
    pos.draw();
}

void Player::handleKeyPressed(char key_pressed) {
    if (!visible) return;

    size_t index = findKeyIndex(key_pressed);
    if (index < NUM_KEYS) {
        // pick/dispose keys handled specially
        char k = keys[index];
        if (k == 'o' || k == 'e') {
            handlePickupOrDispose(index);
        } else {
            setDirectionByIndex(index);
        }
    }
}


// --- private helpers ---

size_t Player::findKeyIndex(char key_pressed) const {
    size_t index = 0;
    for (char k : keys) {
        if (tolower(static_cast<unsigned char>(k)) == tolower(static_cast<unsigned char>(key_pressed))) {
            return index;
        }
        index++;
    }
    return NUM_KEYS;
}

void Player::handlePickupOrDispose(size_t /*index*/) {
    if (inventory == nullptr) {
        pickUp();
        // stay in place after pickup
        pos.setDirection(Direction::STAY);
    }
    else {
        dispose();
    }
}

void Player::setDirectionByIndex(size_t index) {
    if (index <= static_cast<size_t>(Direction::STAY)) {
        pos.setDirection(static_cast<Direction>(index));
    }
}

bool Player::canMoveTo(const Point& next) const {
    // block on walls/doors/items (like wall collision)
    if (screen.isWall(next) || screen.isItem(next))
        return false;
    return true;
}

void Player::updateTorchOnMove(const Point& next) {
    Torch* torch = nullptr;
    if (inventory) torch = dynamic_cast<Torch*>(inventory);
    if (torch) {
        torch->paintLightDiff(screen, pos, next);
    }
}
