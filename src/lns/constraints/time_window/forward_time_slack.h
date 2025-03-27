#pragma once

#include "lns/solution/route.h"
#include "types.h"

/*
 *  Represent the forward time slack of a route.
 */
class ForwardTimeSlack
{
private:
    std::vector<TimeInteger> earliestArrival;
    std::vector<TimeInteger> latestArrival;
    std::vector<TimeInteger> FTS;

public:
    explicit ForwardTimeSlack() = default;

    /*
     *  Check if the insertion of a new pickup/delivery pair is feasible.
     *  ForwardTimeSlack is supposed to be correct. Only check FTS and Time windows.
     */
    bool isPickupDeliveryInsertionValid(PDPTWData const &data, Route const &route, int pickupID, int deliveryID,
                                        int insertPickupIndex, int insertDeliveryIndex) const;

    /*
     *  Compute FTS values from scratch.
     */
    void initFTS(PDPTWData const &data, Route const &route);

    /*
     *  Update FTS.
     *  Suppose the route has already been updated (but not the FTS)
     */
    void updateFTSAfterInsertion(PDPTWData const &data, Route const &route, int insertPickupIndex,
                                 int insertDeliveryIndex);
    /*
     *  Update FTS.
     *  Suppose the route has already been updated (but not the FTS)
     */
    void updateFTSAfterDeletion(PDPTWData const &data, Route const &route, int removePickupIndex,
        int removeDeliveryIndex);


    std::vector<TimeInteger> const &getFTS() const;
    std::vector<TimeInteger> const &getEarliestArrival() const;
    std::vector<TimeInteger> const &getLatestArrival() const;

    void print() const;
};