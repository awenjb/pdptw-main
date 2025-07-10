#pragma once

#include "lns/solution/route.h"
#include "types.h"

/*
 * Represents and manages the Forward Time Slack (FTS) of a route.
 * FTS helps determine whether time window constraints are respected 
 * when modifying routes (e.g., inserting or removing requests).
 */
class ForwardTimeSlack
{
private:
    std::vector<TimeInteger> earliestArrival;// Earliest arrival times at each location in the route.
    std::vector<TimeInteger> latestArrival;  // Latest arrival times allowed at each location in the route.
    /**
     * Forward Time Slack values at each location.
     * Represents the flexibility remaining at each step of the route.
     */
    std::vector<TimeInteger> FTS;

public:
    explicit ForwardTimeSlack() = default;

    /**
     * Checks whether inserting a pickup/delivery pair at specified positions
     * respects the current time windows and forward slack constraints.
     * Assumes that the FTS has already been computed and is up to date.
     */
    bool isPickupDeliveryInsertionValid(PDPTWData const &data, Route const &route, int pickupID, int deliveryID,
                                        int insertPickupIndex, int insertDeliveryIndex) const;

    /**
     * Updates FTS after a new pickup and delivery have been inserted into the route.
     * Assumes the route is already updated but FTS values are not.
     */
    void updateFTSAfterInsertion(PDPTWData const &data, Route const &route, int insertPickupIndex,
                                 int insertDeliveryIndex);

    /**
     * Updates FTS after a pickup and delivery have been removed from the route.
     * Assumes the route is already updated but FTS values are not.
     */
    void updateFTSAfterDeletion(PDPTWData const &data, Route const &route, int removePickupIndex,
                                int removeDeliveryIndex);

    /**
     * Updates FTS after a new pickup and delivery have been inserted into the route (consider load-dependent travel time).
     * Assumes the route is already updated but FTS values are not.
     */
    void updateFTSAfterInsertionLTT(PDPTWData const &data, Route const &route, int insertPickupIndex,
                                    int insertDeliveryIndex);

    /**
     * Updates FTS after a pickup and delivery have been removed from the route (consider load-dependent travel time).
     * Assumes the route is already updated but FTS values are not.
     */
    void updateFTSAfterDeletionLTT(PDPTWData const &data, Route const &route, int removePickupIndex,
                                   int removeDeliveryIndex);


    std::pair<double, double> getDelays(PDPTWData const &data, Route const &route, int pickupID,
                                                    int deliveryID, int insertPickupIndex,
                                                    int insertDeliveryIndex) const;


    std::vector<TimeInteger> const &getFTS() const;
    std::vector<TimeInteger> const &getEarliestArrival() const;
    std::vector<TimeInteger> const &getLatestArrival() const;

    void print() const;
};