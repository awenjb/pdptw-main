#include "data.h"



double data::addedCostForInsertion(PDPTWData const &data, int before, int toInsert, int after)
{
    const Matrix & matrix = data.getMatrix();
    double cost = 0;
    cost = matrix.at(before).at(toInsert) + matrix.at(toInsert).at(after) - matrix.at(before).at(after);
    return cost;
}

double data::removedCostForSuppression(PDPTWData const &data, int before, int toRemove, int after)
{
    const Matrix & matrix = data.getMatrix();
    double cost = 0;

    cost = matrix.at(before).at(after) - matrix.at(before).at(toRemove) - matrix.at(toRemove).at(after);
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
    cost +=  matrix.at(0).at(routeIDs.at(0));
    //std::cout << "\n route cost : " << matrix.at(0).at(routeIDs.at(0) << " ";
    cost +=  matrix.at(routeIDs.back()).at(0);
    
    // cost in the route
    for (size_t i = 0; i < routeIDs.size() - 1; ++i) {
        cost += matrix.at(routeIDs.at(i)).at(routeIDs.at(i+1));
        //std::cout << matrix.at(routeIDs.at(i).at(routeIDs.at(i+1) << " ";
    }
    //std::cout << matrix.at(routeIDs.back()).at(0) << " : " << cost << "\n";

    return cost;
}

double data::TravelCost(PDPTWData const &data, int from, int to)
{
    return data.getMatrix().at(from).at(to);
}

double data::TravelTime(PDPTWData const &data, int from, int to)
{
    return data.getMatrix().at(from).at(to);
}


double data::SegmentCost(PDPTWData const &data, Route const &route, int start, int end)
{
    // TO DO
    return 0;
}