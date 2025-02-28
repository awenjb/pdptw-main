#pragma once
#include <vector>

#include "lns/constraints/constraint.h"

#include "lns/solution/solution.h"
#include "lns/modification/pair/insert_pair.h"
#include "lns/modification/pair/remove_pair.h"
#include "lns/modification/route/insert_route.h"
#include "lns/modification/route/remove_route.h"

/**
 * Capacity constraint.
 * To check this, we keep track of the sum of all requests and check it against the max capacity.
 * TO DO, verification in O(1) by storing a matrix !
 */
class CapacityConstraint : public Constraint
{
public:
    using CapacityVector = std::vector<int>;
private:

    std::vector<CapacityVector> routeCapacities;

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
    *   Given the solution, calculate the capacities when leaving each location
    *   Modify the routeCapacities vector !
    */
    void initCapacities();

    /*
    *   Return capacity vector of a route
    */
    CapacityVector const &getRouteCapacities(int routeIndex) const;

    /*
    *   Check if a modification is valid or not
    *   (for now, check only the position of the pickup)
    *   TO DO (1 or 2)
    *   1) check if every index on the path can take the new capacity
    *   2) store a matrix / Segment Tree ?? to evaluate the maximum load between two part of the route 
    *      -> a matrix but check in O(1) (update in O(n))
    */
    bool checkModif(Pair const &pair, int routeIndex, int position, int DeliveryPosition) const;

    /*
    *   Update the weight
    */
    void applyModif(Pair const &pair, int routeIndex, int PickupPosition,  int DeliveryPosition, bool addPair);

    void print() const;
};