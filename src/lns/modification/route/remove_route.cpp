#include "remove_route.h"

#include "input/location.h"

#include <algorithm>
#include <ranges>
#include <utility>
#include <vector>

RemoveRoute::RemoveRoute() : routeIndex(-1), removedPairID({}) {}

RemoveRoute::RemoveRoute(int routeIndex) : routeIndex(routeIndex), removedPairID({}) {}

RemoveRoute::RemoveRoute(int routeIndex, std::vector<int> &&removedPairID)
    : routeIndex(routeIndex), removedPairID(std::move(removedPairID))
{}

void RemoveRoute::modifySolution(Solution &solution)
{
    std::vector<Route> &routes = solution.getRoutes();
    if (routes.at(routeIndex).getRoute().empty())
    {
        std::vector<int> const &locationIDs = routes.at(routeIndex).getRoute();

        // update removedPairID
        removedPairID.reserve(routes.at(routeIndex).getSize());

        for (int id: locationIDs)
        {
            if (solution.getData().getLocation(id).getLocType() == LocType::PICKUP)
            {
                removedPairID.push_back(id);
            }
        }
    }
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

std::vector<int> const &RemoveRoute::getDeletedPairs() const
{
    return removedPairID;
}

ModificationApplyVariant RemoveRoute::asApplyVariant() const
{
    return *this;
}