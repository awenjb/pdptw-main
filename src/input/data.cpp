#include "data.h"



double data::addedCostForInsertion(PDPTWData const &data, int before, int toInsert, int after)
{
    const Matrix & matrix = data.getMatrix();
    double cost = 0;
    cost = matrix[before][toInsert] + matrix[toInsert][after] - matrix[before][after];
    return cost;
}

double data::removedCostForSuppression(PDPTWData const &data, int before, int toRemove, int after)
{
    const Matrix & matrix = data.getMatrix();
    double cost = 0;

    cost = matrix[before][after] - matrix[before][toRemove] - matrix[toRemove][after];
    return cost;
}


double data::routeCost(PDPTWData const & data, Route const & route)
{
    const Matrix & matrix = data.getMatrix();
    const std::vector<int> & routeIDs = route.getRoute();
    double cost = 0;

    if (routeIDs.empty())
    {
        return 0;
    }
    // cost from and to the depot
    cost +=  matrix[0][routeIDs.at(0)];
    //std::cout << "\n route cost : " << matrix[0][routeIDs[0]] << " ";
    cost +=  matrix[routeIDs.back()][0];
    
    // cost in the route
    for (size_t i = 0; i < routeIDs.size() - 1; ++i) {
        cost += matrix[routeIDs[i]][routeIDs[i+1]];
        //std::cout << matrix[routeIDs[i]][routeIDs[i+1]] << " ";
    }
    //std::cout << matrix[routeIDs.back()][0] << " : " << cost << "\n";

    return cost;
}

double data::TravelCost(PDPTWData const &data, int from, int to)
{
    return data.getMatrix()[from][to];
}

double data::TravelTime(PDPTWData const &data, int from, int to)
{
    return data.getMatrix()[from][to];
}


double data::SegmentCost(PDPTWData const &data, Route const &route, int start, int end)
{
    // TO DO
    return 0;
}