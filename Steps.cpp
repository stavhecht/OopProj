#include <fstream>

#include "Steps.h"

Steps Steps::loadSteps(const std::string& filename) {
    Steps out;
    std::ifstream steps_file(filename);
    if (!steps_file.is_open()) return out;
    steps_file >> out.randomSeed;
    size_t count = 0;
    steps_file >> count;
    while (count-- != 0 && !steps_file.eof()) {
        size_t iteration; int player; int k;
        steps_file >> iteration >> player >> k;
        out.addStep(iteration, player, static_cast<char>(k));
    }
    steps_file.close();
    return out;
}

void Steps::saveSteps(const std::string& filename) const {
    std::ofstream steps_file(filename);
    if (!steps_file.is_open()) return;
    steps_file << randomSeed << '\n' << steps.size() << '\n';
    for (const auto& step : steps) {
        steps_file << step.iteration << ' ' << step.player << ' ' << static_cast<int>(step.key) << '\n';
    }
    steps_file.close();
}