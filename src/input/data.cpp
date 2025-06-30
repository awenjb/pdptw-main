#include "data.h"

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