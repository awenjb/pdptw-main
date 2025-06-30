#include "lns_output.h"

output::LnsOutput::LnsOutput(Solution const &bestSolution, unsigned int numberOfIteration,
                     unsigned int numberOfIterationFleetMin, unsigned long timeSpent, unsigned long timeSpentFleetMin,
                     unsigned int bestIterationFleetMin, unsigned long bestTimeFleetMin, unsigned int bestIteration,
                     unsigned long bestTime, std::vector<unsigned long> const &bestTimes,
                     std::vector<int> const &bestIterations, std::vector<int> const &bestVehicles,
                     std::vector<double> const &bestCosts, unsigned long totalTravelTime)
    : bestSolution(bestSolution), numberOfIteration(numberOfIteration),
      numberOfIterationFleetMin(numberOfIterationFleetMin), timeSpent(timeSpent), timeSpentFleetMin(timeSpentFleetMin),
      bestIterationFleetMin(bestIterationFleetMin), bestTimeFleetMin(bestTimeFleetMin), bestIteration(bestIteration),
      bestTime(bestTime), bestTimes(bestTimes), bestIterations(bestIterations), bestVehicles(bestVehicles),
      bestCosts(bestCosts), totalTravelTime(totalTravelTime)
{}

Solution output::LnsOutput::getBestSolution() const
{
    return bestSolution;
}

unsigned int output::LnsOutput::getNumberOfIteration() const
{
    return numberOfIteration;
}

unsigned int output::LnsOutput::getNumberOfIterationFleetMin() const
{
    return numberOfIterationFleetMin;
}

unsigned long output::LnsOutput::getTimeSpent() const
{
    return timeSpent;
}

unsigned long output::LnsOutput::getTimeSpentFleetMin() const
{
    return timeSpentFleetMin;
}

unsigned int output::LnsOutput::getBestIterationFleetMin() const
{
    return bestIterationFleetMin;
}

unsigned long output::LnsOutput::getBestTimeFleetMin() const
{
    return bestTimeFleetMin;
}

unsigned int output::LnsOutput::getBestIteration() const
{
    return bestIteration;
}

unsigned long output::LnsOutput::getBestTime() const
{
    return bestTime;
}

std::vector<unsigned long> const &output::LnsOutput::getBestTimes() const
{
    return bestTimes;
}

std::vector<int> const &output::LnsOutput::getBestIterations() const
{
    return bestIterations;
}

std::vector<int> const &output::LnsOutput::getBestVehicles() const
{
    return bestVehicles;
}

std::vector<double> const &output::LnsOutput::getBestCosts() const
{
    return bestCosts;
}

unsigned long output::LnsOutput::getTotalTravelTime() const
{
    return totalTravelTime;
}
