#include "data.h"

#include "config.h"
#include "input/load_dependent.h"
#include "lns/solution/solution.h"

#include <math.h>

double data::addedCostForInsertion(PDPTWData const &data, int before, int toInsert, int after)
{
    Matrix const &matrix = data.getMatrix();
    return matrix.at(before).at(toInsert) + matrix.at(toInsert).at(after) - matrix.at(before).at(after);
}

double data::removedCostForSuppression(PDPTWData const &data, int before, int toRemove, int after)
{
    Matrix const &matrix = data.getMatrix();
    double cost = matrix.at(before).at(after) - matrix.at(before).at(toRemove) - matrix.at(toRemove).at(after);
    return cost;
}

double data::routeCost(PDPTWData const &data, Route const &route)
{
    // In load-dependent mode, the objective actually optimized must be the real
    // load-dependent travel time, not the raw distance matrix.
    if (ELEVATION)
    {
        return routeTravelTimeLTT(data, route.getRoute());
    }

    Matrix const &matrix = data.getMatrix();
    std::vector<int> const &routeIDs = route.getRoute();
    double cost = 0.0;

    if (routeIDs.empty())
    {
        return 0.0;
    }
    // cost from and to the depot
    cost += matrix.at(0).at(routeIDs.at(0));
    cost += matrix.at(routeIDs.back()).at(0);

    // cost in the route
    for (size_t i = 0; i < routeIDs.size() - 1; ++i)
    {
        int from = routeIDs.at(i);
        int to = routeIDs.at(i + 1);
        cost += matrix.at(from).at(to);
    }

    return cost;
}

double data::travelCost(PDPTWData const &data, int from, int to)
{
    return data.getMatrix().at(from).at(to);
}

double data::totalTravelTime(PDPTWData const &data, Solution const &sol)
{
    // compute the minimum travel time (time in movement) for every route
    double total = 0.0;

    for (Route const &route: sol.getRoutes())
    {
        std::vector<int> const &routeIDs = route.getRoute();
        double load = 0.0;
        int prev = 0;

        for (int curr: routeIDs)
        {
            load += data.getLocation(curr).getDemand();
            total += ltt::getTravelTimeLTT(data, load, prev, curr);
            // total +=  ltt::fontaineCalculation(data.getSegmentDistanceMatrix().at(prev).at(curr),
            //                          data.getSegmentSlopeMatrix().at(prev).at(curr),
            //                          load);
            // total += data.getMatrix().at(prev).at(curr) / 6.94444;

            prev = curr;
        }


        if (!routeIDs.empty())
        {
            total += ltt::getTravelTimeLTT(data, 0, prev, 0);
            // total += ltt::fontaineCalculation(data.getSegmentDistanceMatrix().at(prev).at(0),
            //                          data.getSegmentSlopeMatrix().at(prev).at(0),
            //                          load);
            // total += data.getMatrix().at(prev).at(0) / 6.94444;
        }
    }

    return total;
}

double data::routeTravelTimeLTT(PDPTWData const &data, std::vector<int> const &routeIDs)
{
    if (routeIDs.empty())
    {
        return 0.0;
    }

    double total = 0.0;
    double load = 0.0;
    int prev = 0;

    for (int curr: routeIDs)
    {
        load += data.getLocation(curr).getDemand();
        total += ltt::getTravelTimeLTT(data, load, prev, curr);
        prev = curr;
    }

    // return to the depot with a load of 0, consistent with data::totalTravelTime
    // (every pickup has its matching delivery inside the route, so the net load back
    // at the depot is always 0 for a valid route).
    total += ltt::getTravelTimeLTT(data, 0, prev, 0);

    return total;
}

double data::addedCostForInsertionLTT(PDPTWData const &data, Route const &route, int pickupID, int deliveryID,
                                      int pickupPos, int deliveryPos)
{
    std::vector<int> const &routeIDs = route.getRoute();
    double oldTime = routeTravelTimeLTT(data, routeIDs);

    // Mirrors InsertPair::modifySolution: pickup inserted first, delivery position is
    // expressed relative to the route *before* the pickup insertion, hence the +1 shift.
    std::vector<int> newRoute = routeIDs;
    newRoute.insert(newRoute.begin() + pickupPos, pickupID);
    newRoute.insert(newRoute.begin() + deliveryPos + 1, deliveryID);

    double newTime = routeTravelTimeLTT(data, newRoute);

    return newTime - oldTime;
}

double data::removedCostForSuppressionLTT(PDPTWData const &data, Route const &route, int pickupPos, int deliveryPos)
{
    std::vector<int> const &routeIDs = route.getRoute();
    double oldTime = routeTravelTimeLTT(data, routeIDs);

    // Mirrors RemovePair::modifySolution: delivery removed first to keep pickupPos valid.
    std::vector<int> newRoute = routeIDs;
    newRoute.erase(newRoute.begin() + deliveryPos);
    newRoute.erase(newRoute.begin() + pickupPos);

    double newTime = routeTravelTimeLTT(data, newRoute);

    return newTime - oldTime;
}