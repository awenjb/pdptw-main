#pragma once

#include "input/pair.h"
#include "input/time_window.h"
#include "lns/constraints/constraint.h"
#include "lns/constraints/time_window/forward_time_slack.h"
#include "lns/solution/route.h"

#include <vector>

/**
 * Constraint that ensures all pickup and delivery respect their time windows (with load-dependent travel time)
 */
class TimeWindowLTTConstraint : public Constraint
{
public:
    using ArrivalTimeVector = std::vector<TimeInteger>;


    explicit TimeWindowLTTConstraint(Solution const &);
    TimeWindowLTTConstraint(TimeWindowLTTConstraint const &) = default;
    ~TimeWindowLTTConstraint() override;

    void initFTS();//Initializes Forward Time Slack vectors for each route in the solution.

    std::unique_ptr<Constraint> clone(Solution const &newOwningSolution) const override;

    void print() const override;

private:
    std::vector<ForwardTimeSlack> FTSContainer;//Stores the FTS (Forward Time Slack) state for each route.

    std::vector<ArrivalTimeVector> arrivalTimeContainer;

    /**
     * Checks whether inserting a pickup/delivery pair into the specified route
     * (at given positions) respects the time window constraints.
     * Assumes the route is copied and not modified in-place.
     */
    bool checkInsertion(PDPTWData const &data, Pair const &pair, int routeIndex, int pickupPos, int deliveryPos) const;

    /**
     * Applies the modification to the FTS structure after insertion or removal of a pair. (TO DO)
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