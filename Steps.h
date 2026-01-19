#pragma once
#include <map>
#include <vector>
#include <string>
#include <list>


// Minimal replay IO: reads adv-world.steps and writes adv-world.result
struct StepEvent {
    std::size_t iteration;    // game cycle
    int player;  // 0 or 1
    char key;    // pressed key
};

class Steps {
    long randomSeed = 0;
    std::list<StepEvent> steps; // sequence of step events
public:
	static Steps loadSteps(const std::string& filename);
	void saveSteps(const std::string& filename) const;
	long getRandomSeed() const {
		return randomSeed;
	}
	void setRandomSeed(long seed) {
		randomSeed = seed;
	}
    void addStep(std::size_t iteration, int player, char step) {
        steps.push_back({ iteration, player, step });
    }
    bool isNextStepOnIteration(std::size_t iteration) const {
        return !steps.empty() && steps.front().iteration == iteration;
    }
    char popStep() {
        char step = steps.front().key;
        steps.pop_front();
        return step;
    }

    // return copy of steps at given iteration
    std::vector<StepEvent> stepsAt(std::size_t iteration) const {
        std::vector<StepEvent> out;
        for (const auto& s : steps) {
            if (s.iteration == iteration) out.push_back(s);
        }
        return out;
    }

    // New: expose number of recorded events
    std::size_t size() const {
        return steps.size();
    }
};