#include "small_large_selector.h"

#include <algorithm>
#include <spdlog/spdlog.h>

SmallLargeOperatorSelector::SmallLargeOperatorSelector(std::vector<StepSelector> selectorsList)
    : iterationAtCurrentStep(0), selectorStep(0), selectorPerSize(std::move(selectorsList))
{
    iterationForNextStep = selectorPerSize.at(0).first;
}

OperatorPair SmallLargeOperatorSelector::getOperatorPair()
{
    if (iterationAtCurrentStep > iterationForNextStep) [[unlikely]]
    {
        ++selectorStep;
        if (selectorStep == selectorPerSize.size())
        {
            betterSolutionFound();
        }
        iterationForNextStep = selectorPerSize.at(selectorStep).first;
    }

    SimpleOperatorSelector &selector = selectorPerSize.at(selectorStep).second;
    OperatorPair pair = selector.getOperatorPair();
    
    // when we arrive at the last iteration of the last selector, we force the acceptance
    if (iterationAtCurrentStep == iterationForNextStep - 1 && selectorStep == selectorPerSize.size()) [[unlikely]]
    {
        pair.setForceAcceptance();
    }
    ++iterationAtCurrentStep;
    return pair;
}

void SmallLargeOperatorSelector::betterSolutionFound()
{
    iterationAtCurrentStep = 0;
    selectorStep = 0;
}
