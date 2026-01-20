#pragma once
#include <string>
#include <vector>
#include <iostream>

struct ResultEvent {
    enum Type { RoomChange, LifeLost, Riddle, GameEnd, Unknown };
    Type type = Unknown;
    size_t iteration = 0;
    int playerIndex = -1;
    int toRoom = -1;      // for RoomChange
    int roomIndex = -1;   // for Riddle
    std::string riddleQ;  // for Riddle
    std::string riddleA;  // for Riddle
    bool riddleCorrect = false;
    int score = 0;        // for GameEnd

    bool operator==(const ResultEvent& other) const {
        if (type != other.type || iteration != other.iteration) return false;
        if (type == RoomChange) return playerIndex == other.playerIndex && toRoom == other.toRoom;
        if (type == LifeLost) return playerIndex == other.playerIndex;
        if (type == Riddle) return roomIndex == other.roomIndex && riddleQ == other.riddleQ && riddleA == other.riddleA && riddleCorrect == other.riddleCorrect;
        if (type == GameEnd) return score == other.score;
        return true;
    }
    bool operator!=(const ResultEvent& other) const { return !(*this == other); }
};

class Results {
    std::vector<ResultEvent> events;
    int resScore = 0; // legacy accessor support
    int resLives = 0; // legacy accessor support

public:
    static Results loadResults(const std::string& filename);
    void saveResults(const std::string& filename) const;

    void addEvent(const ResultEvent& e) {
        events.push_back(e);
        // keep legacy scores updated if GameEnd
        if (e.type == ResultEvent::GameEnd) resScore = e.score;
    }

    const std::vector<ResultEvent>& getEvents() const { return events; }

    // Legacy getters/setters for ReplayIO compatibility if needed
    // (We will primarily use the events vector for validation)
    int getScore() const { return resScore; }
    void changeScore(int s) { resScore = s; } 
    int getLives() const { return resLives; } // likely unused
    
    // Check if recorded events match
    bool operator==(const Results& other) const {
        if (events.size() != other.events.size()) return false;
        for (size_t i = 0; i < events.size(); ++i) {
            if (events[i] != other.events[i]) return false;
        }
        return true;
    }
};