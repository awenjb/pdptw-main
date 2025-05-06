#pragma once

#include "input/pair.h"
#include "input/time_window.h"
#include "lns/constraints/constraint.h"
#include "lns/constraints/time_window/forward_time_slack.h"
#include "lns/solution/route.h"

#include <vector>

/**
 * Time Window Constraint
 * Check that the time windows are respected.
 */
class TimeWindowConstraint : public Constraint
{
public:
    using ArrivalTimeVector = std::vector<TimeInteger>;


    explicit TimeWindowConstraint(Solution const &);
    TimeWindowConstraint(TimeWindowConstraint const &) = default;
    ~TimeWindowConstraint() override;


    /**
     *  Initialize the FTS vectors
     */
    void initFTS();

    std::unique_ptr<Constraint> clone(Solution const &newOwningSolution) const override;


    void print() const override;

private:
    std::vector<ForwardTimeSlack> FTSContainer;

    /* Debut Ajout pour calcul sans FTS */ 
    std::vector<ArrivalTimeVector> arrivalTimeContainer;

    /* Fin Ajout pour calcul sans FTS */

    /**
     *  Check if the insertion of a pair pickup/delivery is valid or not.
     *  COPY the route where we insert the pair !
     */
    bool checkInsertion(PDPTWData const &data, Pair const &pair, int routeIndex, int pickupPos, int deliveryPos) const;

    /**
     *  Apply the modification to the time window constraint
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