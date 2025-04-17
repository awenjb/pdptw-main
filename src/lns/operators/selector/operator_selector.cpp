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

OperatorPair::OperatorPair(DestructionOperator &destruct, ReconstructionOperator &reconstruct, bool forceAcceptance,
                           bool largeIteration)
    : destruct(destruct), reconstruct(reconstruct), forceAcceptance(forceAcceptance)
{}

void OperatorPair::setForceAcceptance()
{
    forceAcceptance = true;
}

OperatorPair SimpleOperatorSelector::getOperatorPair()
{
    return {*destructOperators.at(destructDistribution(util::getRawRandom())),
            *reconstructOperators.at(reconstructDistribution(util::getRawRandom()))};
}
