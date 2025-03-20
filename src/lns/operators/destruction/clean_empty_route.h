#pragma once

#include "lns/operators/abstract_operator.h"

class CleanEmptyRoute : public DestructionOperator
{
public:
    explicit CleanEmptyRoute();

    /**
     * This operator removes empty routes from the solution.
     */
    void destroySolution(Solution &solution) const override;

};
