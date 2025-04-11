#include "min_small_large_selector.h"

#include "config.h"

#include <algorithm>
#include <spdlog/spdlog.h>

MinSmallLargeOperatorSelector::MinSmallLargeOperatorSelector(std::vector<SimpleOperatorSelector> selectorsList,
                                                             int totalIteration)
    : totalIteration(totalIteration), SLNSIteration(0), selectorStep(0),
      operatorList(std::move(selectorsList))
{}

OperatorPair MinSmallLargeOperatorSelector::getOperatorPair()
{
    int smallToLarge = totalIteration * LNS_FREQUENCY;

    if (SLNSIteration < smallToLarge)
    {
        // Small
        selectorStep = 0;
    }
    else
    {
        // Large
        selectorStep = 1;
        SLNSIteration = 0;
    }
    ++SLNSIteration;

    SimpleOperatorSelector &selector = operatorList.at(selectorStep);
    OperatorPair pair = selector.getOperatorPair();

    return pair;
}

void MinSmallLargeOperatorSelector::betterSolutionFound()
{
    // print
    if (selectorStep == 0)
    {
        std::cout << "in small phase" << std::endl;
    }
    if (selectorStep == 1)
    {
        std::cout << "in large phase" << std::endl;
    }

    SLNSIteration = 0;
}
