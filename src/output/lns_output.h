#pragma once

#include "lns/solution/solution.h"

namespace output
{

    class LnsOutput
    {
    private:
        Solution bestSolution;
        unsigned int numberOfIteration;
        unsigned int numberOfIterationFleetMin;
        unsigned long timeSpent;
        unsigned long timeSpentFleetMin;

        unsigned int bestIterationFleetMin;
        unsigned long bestTimeFleetMin;

        unsigned int bestIteration;
        unsigned long bestTime;

        std::vector<unsigned long> bestTimes;
        std::vector<int> bestIterations;
        std::vector<int> bestVehicles;
        std::vector<double> bestCosts;

    public:
        // Constructeur
        LnsOutput(Solution const &bestSolution, unsigned int numberOfIteration, unsigned int numberOfIterationFleetMin,
                  unsigned long timeSpent, unsigned long timeSpentFleetMin, unsigned int bestIterationFleetMin,
                  unsigned long bestTimeFleetMin, unsigned int bestIteration, unsigned long bestTime,
                  std::vector<unsigned long> const &bestTimes, std::vector<int> const &bestIterations,
                  std::vector<int> const &bestVehicles, std::vector<double> const &bestCost);

        // Getters
        Solution getBestSolution() const;
        unsigned int getNumberOfIteration() const;
        unsigned int getNumberOfIterationFleetMin() const;
        unsigned long getTimeSpent() const;
        unsigned long getTimeSpentFleetMin() const;

        unsigned int getBestIterationFleetMin() const;
        unsigned long getBestTimeFleetMin() const;

        unsigned int getBestIteration() const;
        unsigned long getBestTime() const;

        std::vector<unsigned long> const &getBestTimes() const;
        std::vector<int> const &getBestIterations() const;
        std::vector<int> const &getBestVehicles() const;
        std::vector<double> const &getBestCosts() const;
    };

}// namespace output