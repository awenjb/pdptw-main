#pragma once

#include "time_window.h"
#include "pdptw_data.h"
#include "location.h"
#include "lns/solution/solution.h"


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
     * Compute the cost of a segment of location in a route
     * (does not take into accout that pair of pickup/delivery must be removed together)
     */
    double SegmentCost(PDPTWData const &data, Route const &route, int start, int end);

    /**
     * Return the travel cost between two location
     * (take location id in parameters)
     */
    double TravelCost(PDPTWData const &data, int from, int to);

    /**
     * Return the travel time between two location
     * (take location id in parameters)
     */
     double TravelTime(PDPTWData const &data, int from, int to);
}