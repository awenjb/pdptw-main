#include "classic_small_large_selector.h"

#include "config.h"

#include <algorithm>
#include <spdlog/spdlog.h>

ClassicSmallLargeOperatorSelector::ClassicSmallLargeOperatorSelector(std::vector<SimpleOperatorSelector> selectorsList,
                                                             int totalIteration)
    : totalIteration(totalIteration), SLNSIteration(0), selectorStep(0),
      operatorList(std::move(selectorsList))
{}

OperatorPair ClassicSmallLargeOperatorSelector::getOperatorPair()
{
    int frequency = totalIteration * LNS_FREQUENCY;

    if (SLNSIteration < frequency)
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

    if (selectorStep == 1) // always accept solution from a large iteration
    {
        pair.setForceAcceptance();
        pair.setLargeIteration();
    }

    return pair;
}

void ClassicSmallLargeOperatorSelector::betterSolutionFound()
{
    SLNSIteration = 0;
}
