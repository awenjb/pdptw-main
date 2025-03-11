#pragma once

#include "lns/operators/reconstruction/enumerate.h"
#include "sorting_strategy.h"

class Solution;

class DestructionOperator
{
public:
    virtual void destroySolution(Solution &solution) const = 0;
    virtual ~DestructionOperator() = default;
};

class ReconstructionOperator
{
public:
    virtual void reconstructSolution(Solution &solution, double blinkRate, SortingStrategyType strategy, EnumerationType enumeration) const = 0;
    virtual ~ReconstructionOperator() = default;
};
