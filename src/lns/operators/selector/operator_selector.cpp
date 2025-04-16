#include "operator_selector.h"

#include "utils.h"

ReconstructionOperator &OperatorPair::reconstructor()
{
    return reconstruct;
}

DestructionOperator &OperatorPair::destructor()
{
    return destruct;
}

bool OperatorPair::forceTakeSolution() const
{
    return forceAcceptance;
}

bool OperatorPair::isLargeIteration()
{
    if (largeIteration)
    {
        largeIteration = false;
        return true;
    }
    return false;
}

OperatorPair::OperatorPair(DestructionOperator &destruct, ReconstructionOperator &reconstruct, bool forceAcceptance,
                           bool largeIteration)
    : destruct(destruct), reconstruct(reconstruct), forceAcceptance(forceAcceptance), largeIteration(largeIteration)
{}

void OperatorPair::setForceAcceptance()
{
    forceAcceptance = true;
}

void OperatorPair::setLargeIteration()
{
    largeIteration = true;
}

OperatorPair SimpleOperatorSelector::getOperatorPair()
{
    return {*destructOperators.at(destructDistribution(util::getRawRandom())),
            *reconstructOperators.at(reconstructDistribution(util::getRawRandom()))};
}
