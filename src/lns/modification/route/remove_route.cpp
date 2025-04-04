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
    std::vector<int> const &routeIDs = routes.at(routeIndex).getRoute();

    if (!(routeIDs.empty()))
    {
        // update removedPairID
        removedPairID.reserve(routeIDs.size());

        for (int id: routeIDs)
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