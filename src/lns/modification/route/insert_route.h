#pragma once

#include "lns/modification/atomic_recreation.h"
#include "lns/solution/solution.h"
#include "input/location.h"

/**
 * Insert a new empty route in the solution
 * Do not cost anything
 */
class InsertRoute : public AtomicRecreation
{

public:
    InsertRoute();

    void modifySolution(Solution &solution) override;
    double evaluate(Solution const &solution) const override;
    Location const *getAddedLocation() const override;

    ModificationCheckVariant asCheckVariant() const override;
};  