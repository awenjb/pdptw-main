#pragma once

#include "input/location.h"
#include "lns/modification/atomic_recreation.h"
#include "lns/solution/solution.h"

class InsertRoute : public AtomicRecreation
{
public:
    InsertRoute();

    void modifySolution(Solution &solution) override;
    double evaluate(Solution const &solution) const override;
    int getAddedPairs() const override;

    ModificationCheckVariant asCheckVariant() const override;
};