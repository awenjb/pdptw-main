#include "remove_route.h"
#include "../../../input/data.h"

RemoveRoute::RemoveRoute() : routeIndex(-1), removedLocationID({}) {}
RemoveRoute::RemoveRoute(int routeIndex) : routeIndex(routeIndex), removedLocationID({}) {}
RemoveRoute::RemoveRoute(int routeIndex, std::vector<int> removedLocationID) : routeIndex(routeIndex), removedLocationID(removedLocationID) {}

void RemoveRoute::modifySolution(Solution &solution)
{
    std::vector<Route> &routes = solution.getRoutes();

    // update removedLocationID
    removedLocationID.insert(removedLocationID.end(), 
    std::make_move_iterator(routes[routeIndex].getRoute().begin()), 
    std::make_move_iterator(routes[routeIndex].getRoute().end()));

    routes.erase(routes.begin() + routeIndex);
}

double RemoveRoute::evaluate(Solution const &solution) const
{
    return data::routeCost(solution.getData(), solution.getRoute(routeIndex));
}

int RemoveRoute::getRouteIndex() const
{
    return routeIndex;
}


std::vector<int> const &RemoveRoute::getDeletedRequests() const
{
    return removedLocationID;
}