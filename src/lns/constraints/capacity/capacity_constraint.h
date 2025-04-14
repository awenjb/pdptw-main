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
 * Capacity constraint.
 */
class CapacityConstraint : public Constraint
{
public:
    using CapacityVector = std::vector<int>;

private:

    /**
     *  Linearise matrix
     *  Store the maximum used capacity between two locations.
     *  A cell is not updated to 0 when the location is removed from a route.
     */
    std::vector<double> maxCapacity;
    /*
     *  number of locations
     */
    int n;

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

    /*
     *  Update MaxCapacity.
     *  Suppose the route has already been updated
     */
    void updateMaxCapacity(Route const &route);

    /*
     *  Return capacity vector of a route
     */
    CapacityVector const &getRouteCapacities(int routeIndex) const;

    bool checkModif(Pair const &pair, int routeIndex, int position, int DeliveryPosition) const;
    void applyModif(Pair const &pair, int routeIndex, int PickupPosition, int DeliveryPosition, bool addPair);

    void print() const override;
};