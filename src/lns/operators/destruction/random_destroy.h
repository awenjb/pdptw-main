#pragma once

#include "lns/operators/abstract_operator.h"

class RandomDestroy : public DestructionOperator
{
public:
    explicit RandomDestroy(int numberOfPairsToDestroy) : numberOfPairsToDestroy(numberOfPairsToDestroy) {}

    /**
     * This operator removes numberOfPairsToDestroy pairs randomly in the solution.
     */
    void destroySolution(Solution &solution) const override;

private:
    int numberOfPairsToDestroy;
};
