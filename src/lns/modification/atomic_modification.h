#pragma once

#include <vector>

#include "lns/constraints/constraint.h"
class Solution;

/**
 * Represent a modification of the solution, it can recreate/destroy the solution.
 * 
 * @see AtomicRecreation and AtomicDestruction
 */
class AtomicModification
{
public:
    virtual ~AtomicModification() = default;

    /**
     * Return the cost of the operation.
     * @return a positive number if the solution cost is bigger after the modification.
     */
    virtual double evaluate(Solution const &solution) const = 0;

     /**
     * Apply of the modification to the solution.
     * @note does not check the validity of the modification, does not update solution cost nor constraints
     */
    virtual void modifySolution(Solution &solution) = 0;
};


