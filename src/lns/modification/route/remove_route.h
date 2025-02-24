#pragma once

#include "./../atomic_destruction.h"
#include "./../../solution/solution.h"
#include "./../../../input/location.h"
#include <functional>


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
    std::vector<int> removedLocationID;

public:

    RemoveRoute();
    RemoveRoute(int routeIndex);
    RemoveRoute(int routeIndex, std::vector<int> removedLocationID);

    void modifySolution(Solution &solution) override;
    double evaluate(Solution const &solution) const override;

    int getRouteIndex() const;

    /**
     *  Return the location ID of location that has been deleted
     */
    std::vector<int> const &getDeletedRequests() const override;
};