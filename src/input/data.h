#pragma once

#include "lns/solution/solution.h"
#include "location.h"
#include "pdptw_data.h"
#include "time_window.h"

/**
 * Functions for PDPTWData
 */
namespace data
{

    /**
     * Compute the cost added by an insertion of one location.
     * Index are the id of the location (the same as in the cost matrix)
     */
    double addedCostForInsertion(PDPTWData const &data, int before, int toInsert, int after);

    /**
     * Compute the gain of removing an location.
     * Index are the id of the location (the same as in the cost matrix)
     */
    double removedCostForSuppression(PDPTWData const &data, int before, int toRemove, int after);

    /**
     * Compute the cost of a route.
     * When ELEVATION is enabled, this returns the real load-dependent travel time of the
     * route (see routeTravelTimeLTT) instead of the distance-based cost, so that the
     * objective actually optimized matches the load-dependent travel time.
     */
    double routeCost(PDPTWData const &data, Route const &route);

    /**
     * Return the travel cost (distance) between two location
     * (take location id in parameters)
     */
    double travelCost(PDPTWData const &data, int from, int to);

    /**
     * Return the total travel time when elevation is used
     */
    double totalTravelTime(PDPTWData const &data, Solution const &sol);

    /**
     * Compute the total load-dependent travel time (Fontaine model) of a route given as a
     * list of location IDs (depot excluded), including the legs from and to the depot.
     * Load is accumulated exactly like in TimeWindowLTTConstraint / totalTravelTime.
     */
    double routeTravelTimeLTT(PDPTWData const &data, std::vector<int> const &routeIDs);

    /**
     * Compute the added cost (load-dependent travel time delta) of inserting a
     * pickup-delivery pair into a route at the given positions.
     * pickupPos/deliveryPos follow the same convention as InsertPair::modifySolution
     * (deliveryPos is expressed before the pickup is inserted, hence the +1 shift).
     */
    double addedCostForInsertionLTT(PDPTWData const &data, Route const &route, int pickupID, int deliveryID,
                                    int pickupPos, int deliveryPos);

    /**
     * Compute the cost delta (load-dependent travel time) of removing a pickup-delivery
     * pair from a route at the given positions (same convention as RemovePair::modifySolution).
     */
    double removedCostForSuppressionLTT(PDPTWData const &data, Route const &route, int pickupPos, int deliveryPos);

}// namespace data