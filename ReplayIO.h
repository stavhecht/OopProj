// ReplayIO.h - header for replay step/result I/O
#pragma once

#include <vector>
#include <string>
#include <cstddef>
#include "Steps.h"
#include "Results.h"

// Globals used by replay-enabled code
struct ReplayIO;
extern ReplayIO* g_replay_io;
extern int g_replay_tick;

struct ReplayIO {
    struct Input { int iteration; int player; char key; };

    ReplayIO() = default;

    void setSeed(long s);
    long getSeed() const;

    void recordStep(int iteration, int player, char key);
    std::vector<Input> stepsAt(int iteration) const;

    bool saveSteps(const std::string& filename) const;
    bool loadSteps(const std::string& filename);

    void logLifeLost(int iteration, int player);
    void logRoomChange(int iteration, int player, int toRoom);
    void logRiddle(int iteration, int room, const std::string& question, const std::string& answer, bool correct);
    void logGameEnd(int iteration, int totalScore);

    bool saveResult(const std::string& filename) const;
    bool validateResult(const std::string& filename) const;

private:
    Steps stepsObj;
    Results resultsObj;
};
