#include "lns_output.h"

#include "output/solution_exporter.h"

unsigned int output::LnsOutput::getNumberOfIteration() const
{
    return numberOfIteration;
}

Solution const &output::LnsOutput::getBestSolution() const
{
    return bestSolution;
}

unsigned long output::LnsOutput::getTimeSpent() const
{
    return timeSpent;
}

output::LnsOutput::LnsOutput(Solution &&bestSolution, unsigned int numberOfIteration, unsigned long timeSpent)
    : bestSolution(std::move(bestSolution)), numberOfIteration(numberOfIteration), timeSpent(timeSpent)
{}
