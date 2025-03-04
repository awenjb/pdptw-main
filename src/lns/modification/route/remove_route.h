#pragma once

#include <functional>

#include "input/data.h"
#include "input/location.h"
#include "lns/modification/atomic_destruction.h"
#include "lns/solution/solution.h"

/**
 * A modification that will remove a route from the solution.
 */
class RemoveRoute : public AtomicDestruction
{

    /**
     * Index of the route to remove
     */
    int routeIndex;

    /**
     * Removed Location ID (empty before ModifySolution is called)
     */
    std::vector<int> removedPairID;

public:

    RemoveRoute();
    explicit RemoveRoute(int routeIndex);
    RemoveRoute(int routeIndex, std::vector<int> &&removedPairID);

    void modifySolution(Solution &solution) override;
    double evaluate(Solution const &solution) const override;
    /**
    *  Return the location ID of location that has been deleted
    */
    std::vector<int> const &getDeletedPairs() const override;

    int getRouteIndex() const;
};