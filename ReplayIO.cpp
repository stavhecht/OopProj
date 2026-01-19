// ReplayIO.cpp

#include "ReplayIO.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <iostream>
#include <string>
using namespace std;

void ReplayIO::setSeed(long s) { stepsObj.setRandomSeed(s); }

long ReplayIO::getSeed() const { return stepsObj.getRandomSeed(); }

void ReplayIO::recordStep(int iteration, int player, char key) {
    stepsObj.addStep(static_cast<size_t>(iteration), player, key);
}

vector<ReplayIO::Input> ReplayIO::stepsAt(int iteration) const {
    vector<ReplayIO::Input> out;
    auto evs = stepsObj.stepsAt(static_cast<size_t>(iteration));
    for (const auto& e : evs) {
        out.push_back({ static_cast<int>(e.iteration), e.player, e.key });
    }
    return out;
}

bool ReplayIO::saveSteps(const string& filename) const {
    stepsObj.saveSteps(filename);
    ifstream f(filename);
    return f.is_open();
}

bool ReplayIO::loadSteps(const string& filename) {
    vector<string> candidates;
    candidates.push_back(filename);
    candidates.push_back(string("data/") + filename);
    candidates.push_back(string(".\\data\\") + filename);
    candidates.push_back(string("..\\data\\") + filename);
    candidates.push_back(string("./") + filename);

    for (const auto& path : candidates) {
        ifstream f(path);
        if (f.is_open()) {
            f.close();
            Steps s = Steps::loadSteps(path);
            stepsObj = s;
            cout << "ReplayIO: loaded steps from '" << path << "' seed=" << stepsObj.getRandomSeed()
                      << " events=" << stepsObj.size() << endl;
            return true;
        }
    }
    return false;
}

void ReplayIO::logLifeLost(int iteration, int player) {
    ResultEvent e;
    e.type = ResultEvent::LifeLost;
    e.iteration = static_cast<size_t>(iteration);
    e.playerIndex = player;
    resultsObj.addEvent(e);
}

void ReplayIO::logRoomChange(int iteration, int player, int toRoom) {
    ResultEvent e;
    e.type = ResultEvent::RoomChange;
    e.iteration = static_cast<size_t>(iteration);
    e.playerIndex = player;
    e.toRoom = toRoom;
    resultsObj.addEvent(e);
}

void ReplayIO::logRiddle(int iteration, int room, const string& question, const string& answer, bool correct) {
    ResultEvent e;
    e.type = ResultEvent::Riddle;
    e.iteration = static_cast<size_t>(iteration);
    e.roomIndex = room;
    e.riddleQ = question;
    e.riddleA = answer;
    e.riddleCorrect = correct;
    resultsObj.addEvent(e);
}

void ReplayIO::logGameEnd(int iteration, int totalScore) {
    ResultEvent e;
    e.type = ResultEvent::GameEnd;
    e.iteration = static_cast<size_t>(iteration);
    e.score = totalScore;
    resultsObj.addEvent(e);
}

bool ReplayIO::saveResult(const string& filename) const {
    resultsObj.saveResults(filename);
    return true;
}

bool ReplayIO::validateResult(const string& filename) const {
    Results expected = Results::loadResults(filename);
    bool match = (expected == resultsObj);
    
    if (match) {
        cout << "Test Passed!\n";
    } else {
        cout << "Test Failed!\n";
        cout << "Recorded events: " << resultsObj.getEvents().size() << "\n";
        cout << "Expected events: " << expected.getEvents().size() << "\n";
    }
    return match;
}
