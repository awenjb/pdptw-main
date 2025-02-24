#pragma once

#include "./../atomic_recreation.h"
#include "./../../solution/solution.h"
#include "./../../../input/location.h"
#include <functional>

class Route;

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
    
};  