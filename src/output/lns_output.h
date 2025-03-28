#pragma once

#include "lns/solution/solution.h"

namespace output
{
    class LnsOutput
    {
        Solution bestSolution;
        unsigned int numberOfIteration;
        unsigned long timeSpent;

    public:
        LnsOutput(Solution &&bestSolution, unsigned int numberOfIteration, unsigned long timeSpent);
        unsigned int getNumberOfIteration() const;
        Solution const &getBestSolution() const;
        unsigned long getTimeSpent() const;
    };
}// namespace output