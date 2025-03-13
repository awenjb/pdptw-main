#pragma once

#include "lns/operators/selector/operator_selector.h"

/**
 *
 */
class SmallLargeOperatorSelector : public OperatorSelector
{
public:
    using StepSelector = std::pair<unsigned int, SimpleOperatorSelector>;

private:
    // the number of call to getOperatorPair (ie. lns iteration) done with the current step
    unsigned int iterationAtCurrentStep;
    // the index of the selector used, called step
    unsigned int selectorStep;
    // number of iterations needed to use the next selector
    unsigned int iterationForNextStep;
    // vector of all the selector and the number of iterations for each
    std::vector<StepSelector> selectorPerSize;

public:
    /**
     * @param selectorPerSize each pair is the maximum of iterations for a given selector.
     * ie. {{300,A},{700,B}} means that the A will be used for 300 iterations,
     * then B is used for 700. Then cycle on A again.
     */
    explicit SmallLargeOperatorSelector(std::vector<StepSelector> selectorPerSize);
    OperatorPair getOperatorPair() override;
    void betterSolutionFound() override;
};

