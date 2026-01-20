#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Results.h"

using namespace std;

Results Results::loadResults(const string& filename) {
    Results res;
    ifstream f(filename);
    if (!f.is_open()) return res;

    string line;
    while (getline(f, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string typeStr;
        ss >> typeStr;

        ResultEvent e;
        if (typeStr == "ROOM_CHANGE") {
            e.type = ResultEvent::RoomChange;
            ss >> e.iteration >> e.playerIndex >> e.toRoom;
        } else if (typeStr == "LIFE_LOST") {
            e.type = ResultEvent::LifeLost;
            ss >> e.iteration >> e.playerIndex;
        } else if (typeStr == "RIDDLE") {
            e.type = ResultEvent::Riddle;
            string temp;
            ss >> e.iteration >> e.roomIndex;
            
            size_t qPos = line.find("|Q");
            size_t aPos = line.find("|A");
            
            if (qPos != string::npos && aPos != string::npos) {
                e.riddleQ = line.substr(qPos + 3, aPos - (qPos + 3)); 
                while (!e.riddleQ.empty() && e.riddleQ.back() == ' ') e.riddleQ.pop_back();

                size_t statusPos = string::npos;
                bool ok = false;
                if ((statusPos = line.find("|OK", aPos)) != string::npos) ok = true;
                else if ((statusPos = line.find("|FAIL", aPos)) != string::npos) ok = false;
                
                if (statusPos != string::npos) {
                   e.riddleA = line.substr(aPos + 3, statusPos - (aPos + 3));
                   while (!e.riddleA.empty() && e.riddleA.back() == ' ') e.riddleA.pop_back();
                   e.riddleCorrect = ok;
                }
            }
        } else if (typeStr == "GAME_END") {
            e.type = ResultEvent::GameEnd;
            ss >> e.iteration >> e.score;
        }

        if (e.type != ResultEvent::Unknown) {
            res.addEvent(e);
        }
    }
    return res;
}

void Results::saveResults(const string& filename) const {
    ofstream f(filename);
    for (const auto& e : events) {
        if (e.type == ResultEvent::RoomChange) {
            f << "ROOM_CHANGE " << e.iteration << " " << e.playerIndex << " " << e.toRoom << "\n";
        } else if (e.type == ResultEvent::LifeLost) {
            f << "LIFE_LOST " << e.iteration << " " << e.playerIndex << "\n";
        } else if (e.type == ResultEvent::Riddle) {
            f << "RIDDLE " << e.iteration << " " << e.roomIndex 
              << " |Q " << e.riddleQ << " |A " << e.riddleA 
              << " " << (e.riddleCorrect ? "|OK" : "|FAIL") << "\n";
        } else if (e.type == ResultEvent::GameEnd) {
            f << "GAME_END " << e.iteration << " " << e.score << "\n";
        }
    }
}