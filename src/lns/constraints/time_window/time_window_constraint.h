#pragma once

#include "input/pair.h"
#include "input/time_window.h"
#include "lns/constraints/constraint.h"
#include "lns/constraints/time_window/forward_time_slack.h"
#include "lns/solution/route.h"

#include <vector>

/**
 * Constraint that ensures all pickup and delivery respect their time windows
 * within a route, using Forward Time Slack (FTS).
 */
class TimeWindowConstraint : public Constraint
{
public:
    using ArrivalTimeVector = std::vector<TimeInteger>;

    explicit TimeWindowConstraint(Solution const &);
    TimeWindowConstraint(TimeWindowConstraint const &) = default;
    ~TimeWindowConstraint() override;

    void initFTS();//Initializes Forward Time Slack vectors for each route in the solution.

    std::unique_ptr<Constraint> clone(Solution const &newOwningSolution) const override;

    void print() const override;

private:
    std::vector<ForwardTimeSlack> FTSContainer;//Stores the FTS (Forward Time Slack) state for each route.

    /**
     * [Optional - Not yet used in FTS] 
     * Stores arrival times at each node in a route.
     */
    std::vector<ArrivalTimeVector> arrivalTimeContainer;


    /**
     * Checks whether inserting a pickup/delivery pair into the specified route
     * (at given positions) respects the time window constraints.
     * Assumes the route is copied and not modified in-place.
     */
    bool checkInsertion(PDPTWData const &data, Pair const &pair, int routeIndex, int pickupPos, int deliveryPos) const;

    /**
     * Applies the modification to the FTS structure after insertion or removal of a pair.
     */
    void ApplyModif(PDPTWData const &data, Pair const &pair, int routeIndex, int pickupPos, int deliveryPos,
                    bool addPair);

    void apply(InsertPair const &op) override;
    void apply(InsertRoute const &op) override;
    void apply(RemovePair const &op) override;
    void apply(RemoveRoute const &op) override;

    bool check(InsertPair const &op) const override;
    bool check(InsertRoute const &op) const override;
    bool check(RemovePair const &op) const override;
    bool check(RemoveRoute const &op) const override;
};