#pragma once

#include "lns/solution/solution.h"
#include <vector>
#include <chrono>

struct LnsRuntimeData
{
    Solution bestSolution;

    std::vector<unsigned long> bestTimes;
    std::vector<int> bestIterations;
    std::vector<int> bestVehicles;
    std::vector<double> bestCosts;

    unsigned int bestIterationFleet = 0;
    unsigned long bestTimeFleet = 0;

    unsigned int bestIteration = 0;
    unsigned long bestTime = 0;

    unsigned int numberOfIteration = 0;

    using lns_clock = std::chrono::high_resolution_clock;
    using lns_time_point = std::chrono::time_point<lns_clock, std::chrono::nanoseconds>;

    lns_time_point start = lns_clock::now();

    unsigned long transitionTime = 0;
    unsigned long transitionIteration = 0;

    explicit LnsRuntimeData(Solution sol);
};