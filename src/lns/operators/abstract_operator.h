#pragma once

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
    virtual void reconstructSolution(Solution &solution, double blinkRate) const = 0;
    virtual ~ReconstructionOperator() = default;
};
