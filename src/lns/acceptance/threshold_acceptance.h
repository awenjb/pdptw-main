#pragma once

#include "acceptance_function.h"

/**
 * Simple implementation of AcceptanceFunction.
 * If the candidate is not better than the best solution, we accept it if the relative difference is under a threshold.
 * ACCEPT If better than the actual solution.
 */
class ThresholdAcceptance : public AcceptanceFunction
{
    double thresholdValue = 1.05;

public:
    /**
     * @param threshold 0.05 means that a solution with a ratio with a solution worse by less than 5% will be accepted
     */
    explicit ThresholdAcceptance(double threshold = 0.05);
    AcceptationStatus operator()(Solution const &candidateSolution, Solution const &actualSolution,
                                 Solution const &bestSolution) const override;
};