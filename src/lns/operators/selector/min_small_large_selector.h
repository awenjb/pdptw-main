#pragma once

#include "lns/operators/selector/operator_selector.h"

/**
 *  Selector for a two phased algorithm.
 *  First phase minimise the number of routes.
 *  Second phase minimise the size of each routes.
 */
class MinSmallLargeOperatorSelector : public OperatorSelector
{
public:

private:
    int totalIteration;
    // the number of call to getOperatorPair (ie. lns iteration) done with the current step
    unsigned int iterationAtCurrentStep;

    unsigned int SLNSIteration;

    // the index of the selector used, called step
    unsigned int selectorStep;
    // vector of all the selector and the number of iterations for each
    std::vector<SimpleOperatorSelector> operatorList;

public:
    /**
     *  Does a certain amount of iteration with an initial type of selector.
     *  Then finish the execution with a classical small and large algorirthm.
     */
    explicit MinSmallLargeOperatorSelector(std::vector<SimpleOperatorSelector> operatorList, int totalIterarion);
    OperatorPair getOperatorPair() override;
    void betterSolutionFound() override;
};
