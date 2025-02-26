#pragma once

#include "lns/constraints/constraint.h"
#include "input/pair.h"
#include "input/time_window.h"
#include <vector>

/**
 * Time Window Constraint
 * Check that the time windows are respected.
 * For now, compute all time window in a route
 * TO DO FTS
 */
class TimeWindowConstraint : public Constraint
{
public:
    explicit TimeWindowConstraint(Solution const &);
    TimeWindowConstraint(TimeWindowConstraint const &) = default;

    std::unique_ptr<Constraint> clone(Solution const &newOwningSolution) const override;
    
private:
    using ReachTimeVector = std::vector<TimeInteger>;
    


    /**
    *   For each route, store the reach time of each location
    */
    std::vector<ReachTimeVector> allRouteReachTimes;  

    // Recalcul tous les reachTimes pour une route
    ReachTimeVector computeReachTimes(const Route& route) const;
    bool updateReachTimes(const PDPTWData& data, Pair & pair,int routeIndex, int pickupPos, int deliveryPos);

    /**
    *   Check if the insertion of a pair pickup/delivery is valid or not.
    *   COPY the route where we insert the pair !
    */
    bool checkInsertion(const PDPTWData& data, const Pair & pair, int routeIndex, int pickupPos, int deliveryPos) const; 

    /**
    *   Given route IDs, compute the reachTimes of the route.
    *   If a vehicule is in advance, set the reach time to start of the timeWindow.
    *   Modify reachTimes vector !
    */
    void computeReachTimes(const PDPTWData& data, const std::vector<int> & routeIDs, ReachTimeVector & reachTimes) const;

    /**
    *   Given route IDs, modify the reachTimes
    */
    void ApplyModif(const PDPTWData& data, const Pair & pair, int routeIndex, int pickupPos, int deliveryPos,  bool addPair);


    void apply(InsertPair const &op) override;
    void apply(InsertRoute const &op) override;
    void apply(RemovePair const &op) override;
    void apply(RemoveRoute const &op) override;
 
    bool check(InsertPair const &op) const override;
    bool check(InsertRoute const &op) const override;
    bool check(RemovePair const &op) const override;
    bool check(RemoveRoute const &op) const override;

public: 

};