#include "time_window_constraint.h"

#include "input/data.h"
#include "input/pdptw_data.h"
#include "input/time_window.h"
#include "lns/modification/pair/insert_pair.h"
#include "lns/modification/pair/remove_pair.h"
#include "lns/modification/route/remove_route.h"
#include "lns/solution/solution.h"

TimeWindowConstraint::TimeWindowConstraint(Solution const &solution) : Constraint(solution)
{
    allRouteReachTimes.clear();
    for (unsigned int i = 0; i < solution.getRoutes().size(); ++i)
    {
        allRouteReachTimes.emplace_back();
    }
}

TimeWindowConstraint::~TimeWindowConstraint()
{
    allRouteReachTimes.clear();
}

std::unique_ptr<Constraint> TimeWindowConstraint::clone(Solution const &newOwningSolution) const
{
    std::unique_ptr<TimeWindowConstraint> clonePtr = std::make_unique<TimeWindowConstraint>(newOwningSolution);
    clonePtr->allRouteReachTimes = allRouteReachTimes;
    return clonePtr;
}

void TimeWindowConstraint::computeReachTimes(PDPTWData const &data, std::vector<int> const &routeIDs,
                                             ReachTimeVector &reachTimes) const
{
    // Adjust the size of reachTimes vector
    reachTimes.resize(routeIDs.size(), 0);
    // Time to the first location
    reachTimes.at(0) = data.getDepot().getTimeWindow().getStart() + data::TravelTime(data, 0, routeIDs.at(0));
    // Compute other reachTimes (max between arrival and start of the time window + previous service time + travel time)
    for (int i = 1; i < routeIDs.size(); ++i)
    {
        TimeInteger travelTime = data::TravelTime(data, routeIDs.at(i - 1), routeIDs.at(i));
        // locations are indexed from 0 to n-1,
        TimeInteger serviceTime = data.getLocation(routeIDs.at(i - 1)).getServiceDuration();
        TimeInteger startTW = data.getLocation(routeIDs.at(i - 1)).getTimeWindow().getStart();

        reachTimes.at(i) = std::max(reachTimes.at(i - 1), startTW) + serviceTime + travelTime;
    }
}

void TimeWindowConstraint::initReachTimes()
{
    allRouteReachTimes = std::vector<ReachTimeVector>();
    int i = 0;
    for (Route const &route: getSolution().getRoutes())
    {
        // init and calculate reach time value
        allRouteReachTimes.emplace_back();
        computeReachTimes(getSolution().getData(), route.getRoute(), allRouteReachTimes.at(i));
        ++i;
    }
}

// Copie le vecteur de reach time de la route concernée
// Insère les positions pickup/delivery
// refait l'ordo sur le nouveau vecteur
bool TimeWindowConstraint::checkInsertion(PDPTWData const &data, Pair const &pair, int routeIndex, int pickupPos,
                                          int deliveryPos) const
{

    ReachTimeVector const &reachTimes = allRouteReachTimes.at(routeIndex);


    // COPY route vector
    std::vector<int> route(getSolution().getRoute(routeIndex).getRoute().begin(),
                           getSolution().getRoute(routeIndex).getRoute().end());
    // COPY reachTimes vector
    ReachTimeVector newReachTimes(reachTimes.begin(), reachTimes.end());

    // Insert pickup and delivery
    route.insert(route.begin() + deliveryPos, pair.getDelivery().getId());
    route.insert(route.begin() + pickupPos, pair.getPickup().getId());


    // std::cout << "\n";
    // for (auto pos : route)
    // {
    //     std::cout << pos << " ";
    // }
    // std::cout << "\n";

    // Compute new reach time
    computeReachTimes(data, route, newReachTimes);

    // std::cout << "\n";
    // for (auto pos : newReachTimes)
    // {
    //     std::cout << pos << " ";
    // }
    // std::cout << "\n";

    // Check Time Windows
    for (int i = 0; i < newReachTimes.size(); ++i)
    {
        if (!data.getLocation(route.at(i)).getTimeWindow().isValid(newReachTimes.at(i)))
        {
            return false;
        }
    }
    return true;
}

// A lot of check here because i had problems with this function
void TimeWindowConstraint::ApplyModif(PDPTWData const &data, Pair const &pair, int routeIndex, int pickupPos,
                                      int deliveryPos, bool addPair)
{
    // Check the routeIndex validity
    if (routeIndex < 0 || routeIndex >= getSolution().getRoutes().size())
    {
        spdlog::error("Error: routeIndex out of bounds ({})", routeIndex);
        return;
    }

    // Copy of the route vector (problem that cause a duplication of the TW)
    std::vector<int> routeIDs = getSolution().getRoute(routeIndex).getRoute();

    // // Check Position validity
    // if (pickupPos < 0 || pickupPos > routeIDs.size() ||
    //     deliveryPos < 0 || deliveryPos > routeIDs.size()) {
    //     spdlog::error("Error: Indices pickupPos ({}) or deliveryPos ({}) are invalid for a route size of {}.",
    //         pickupPos, deliveryPos, routeIDs.size());
    //     return;
    // }

    // if (addPair) {
    //     routeIDs.insert(routeIDs.begin() + deliveryPos, pair.getDelivery().getId());
    //     routeIDs.insert(routeIDs.begin() + pickupPos, pair.getPickup().getId());
    // }
    // else {
    //     if (deliveryPos < routeIDs.size() && pickupPos < routeIDs.size() && pickupPos != deliveryPos) {
    //         if (deliveryPos > pickupPos) {
    //             routeIDs.erase(routeIDs.begin() + deliveryPos);
    //             routeIDs.erase(routeIDs.begin() + pickupPos);
    //         } else {
    //             routeIDs.erase(routeIDs.begin() + pickupPos);
    //             routeIDs.erase(routeIDs.begin() + deliveryPos);
    //         }
    //     } else {
    //         spdlog::error("Error: Invalid indices for removal (pickupPos: {}, deliveryPos: {}).", pickupPos, deliveryPos);
    //         return;
    //     }
    // }

    // the route is empty !
    if (routeIDs.empty())
    {
        allRouteReachTimes.at(routeIndex).clear();
    }
    else
    {
        allRouteReachTimes.at(routeIndex).resize(routeIDs.size(), 0);

        allRouteReachTimes.at(routeIndex).at(0) =
                data.getDepot().getTimeWindow().getStart() + data::TravelTime(data, 0, routeIDs.at(0));

        // Compute reach times
        for (size_t i = 1; i < routeIDs.size(); ++i)
        {
            if (i - 1 >= routeIDs.size() || i >= routeIDs.size())
            {
                spdlog::error("Error: Out-of-bounds access to routeIDs in the computation loop.");
                return;
            }

            TimeInteger travelTime = data::TravelTime(data, routeIDs.at(i - 1), routeIDs.at(i));
            TimeInteger serviceTime = data.getLocation(routeIDs.at(i - 1)).getServiceDuration();
            TimeInteger startTW = data.getLocation(routeIDs.at(i - 1)).getTimeWindow().getStart();

            allRouteReachTimes.at(routeIndex).at(i) =
                    std::max(allRouteReachTimes.at(routeIndex).at(i - 1), startTW) + serviceTime + travelTime;
        }
    }
}

bool TimeWindowConstraint::check(InsertPair const &op) const
{
    //std::cout << " #TW Check";
    return checkInsertion(getSolution().getData(),
                          op.getPair(),
                          op.getRouteIndex(),
                          op.getPickupInsertion(),
                          op.getDeliveryInsertion());
}

void TimeWindowConstraint::apply(InsertPair const &op)
{
    //std::cout << "-> Apply Modification on Time Windows \n";
    ApplyModif(getSolution().getData(),
               op.getPair(),
               op.getRouteIndex(),
               op.getPickupInsertion(),
               op.getDeliveryInsertion(),
               true);
}

bool TimeWindowConstraint::check(InsertRoute const &op) const
{
    return true;
}

void TimeWindowConstraint::apply(InsertRoute const &op)
{
    allRouteReachTimes.emplace_back();
}

bool TimeWindowConstraint::check(RemovePair const &op) const
{
    return true;
}

void TimeWindowConstraint::apply(RemovePair const &op)
{
    ApplyModif(getSolution().getData(),
               op.getPair(),
               op.getRouteIndex(),
               op.getPickupDeletion(),
               op.getDeliveryDeletion(),
               false);
}

bool TimeWindowConstraint::check(RemoveRoute const &op) const
{
    return true;
}

void TimeWindowConstraint::apply(RemoveRoute const &op)
{
    allRouteReachTimes.erase(allRouteReachTimes.begin() + op.getRouteIndex());
}

std::vector<TimeWindowConstraint::ReachTimeVector> const &TimeWindowConstraint::getallRouteReachTimes() const
{
    return allRouteReachTimes;
}

void TimeWindowConstraint::print() const
{
    std::cout << "Reach Times : \n";
    int i = 0;
    for (auto const &reachTimes: getallRouteReachTimes())
    {
        //std::cout << reachTimes.size() << ", ";
        std::cout << "#" << i << " : ";
        for (const TimeInteger reachTime: reachTimes)
        {
            std::cout << reachTime << ", ";
        }
        std::cout << "\n";
        i++;
    }
}
