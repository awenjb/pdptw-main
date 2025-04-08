#include "min_small_large_selector.h"

#include "config.h"

#include <algorithm>
#include <spdlog/spdlog.h>

MinSmallLargeOperatorSelector::MinSmallLargeOperatorSelector(std::vector<SimpleOperatorSelector> selectorsList,
                                                             int totalIteration)
    : totalIteration(totalIteration), iterationAtCurrentStep(0), SLNSIteration(0), selectorStep(0), operatorList(std::move(selectorsList))
{}

OperatorPair MinSmallLargeOperatorSelector::getOperatorPair()
{
    int minToSLNS = totalIteration * FIRST_PHASE_ITERATION;
    int smallToLarge = totalIteration * LNS_FREQUENCY;

    // first phase (assume this phase use a route minimisation operator)
    if (iterationAtCurrentStep <= minToSLNS)
    {
        //std::cout << "Phase mini" << std::endl;
        selectorStep = 0;
    }
    else
    {
        // second phase (small and large neighborhood search)

        if (SLNSIteration < smallToLarge)
        {
            // Small
            // std::cout << "Phase small" << std::endl;
            selectorStep = 1;
        }
        else
        {
            // Large
            // std::cout << "Phase large" << std::endl;
            selectorStep = 2;
        }
        ++SLNSIteration;
    }

    SimpleOperatorSelector &selector = operatorList.at(selectorStep);
    OperatorPair pair = selector.getOperatorPair();

    ++iterationAtCurrentStep;
    return pair;
}

void MinSmallLargeOperatorSelector::betterSolutionFound()
{
    if (selectorStep == 0)
    {
        std::cout << "in min phase" << std::endl;
    }
    if (selectorStep == 1)
    {
        std::cout << "in small phase" << std::endl;
    }
    if (selectorStep == 2)
    {
        std::cout << "in large phase" << std::endl;
    }
    SLNSIteration = 0;
}
