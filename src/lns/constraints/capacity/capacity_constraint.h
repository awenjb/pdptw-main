#pragma once
#include "lns/constraints/constraint.h"
#include "lns/modification/pair/insert_pair.h"
#include "lns/modification/pair/remove_pair.h"
#include "lns/modification/route/insert_route.h"
#include "lns/modification/route/remove_route.h"
#include "lns/solution/solution.h"
#include "types.h"

#include <vector>

/**
 * Enforces capacity constraints on routes.
 * Tracks the maximum used capacity between pairs of locations.
 */
class CapacityConstraint : public Constraint
{
public:
    using CapacityVector = std::vector<int>;

private:
    /**
     * Matrix storing maximum capacity used between two locations.
     * Note: When a location is removed from a route, its corresponding maxCapacity
     * cells are NOT reset to zero.
     */
    std::vector<double> maxCapacity;

    int n;//number of locations

    void apply(InsertPair const &op) override;
    void apply(InsertRoute const &op) override;
    void apply(RemovePair const &op) override;
    void apply(RemoveRoute const &op) override;

    bool check(InsertPair const &op) const override;
    bool check(InsertRoute const &op) const override;
    bool check(RemovePair const &op) const override;
    bool check(RemoveRoute const &op) const override;

public:
    explicit CapacityConstraint(Solution const &);
    CapacityConstraint(CapacityConstraint const &) = default;

    std::unique_ptr<Constraint> clone(Solution const &newOwningSolution) const override;

    /**
     * Update the maxCapacity matrix for a given route.
     * Assumes the route has already been updated.
     */
    void updateMaxCapacity(Route const &route);

    /**
     * Get the capacity vector (max capacities) for a specific route.
     */
    CapacityVector const &getRouteCapacities(int routeIndex) const;

    bool checkModif(Pair const &pair, int routeIndex, int position, int DeliveryPosition) const;
    void applyModif(Pair const &pair, int routeIndex, int PickupPosition, int DeliveryPosition, bool addPair);

    void print() const override;
};