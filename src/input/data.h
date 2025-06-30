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
     * Compute the cost of a route
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
    
}// namespace data