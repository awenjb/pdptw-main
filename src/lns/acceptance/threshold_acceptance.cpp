#include "threshold_acceptance.h"

#include "lns/solution/solution.h"

AcceptationStatus ThresholdAcceptance::operator()(Solution const &candidateSolution, Solution const &actualSolution,
                                                  Solution const &bestSolution) const
{
    // the threshold does not take into account the penalisation cost.
    if (candidateSolution.getCost() < bestSolution.getRawCost() * thresholdValue + bestSolution.computePenalisation())
    {
        // accept because of threshold on best solution
        return AcceptationStatus::ACCEPT;
    }
    if (candidateSolution.getCost() < actualSolution.getCost())
    {
        // accept because better than the actual solution
        return AcceptationStatus::ACCEPT;
    }
    return AcceptationStatus::REFUSE;
}

ThresholdAcceptance::ThresholdAcceptance(double threshold) : thresholdValue(1 + threshold) {}