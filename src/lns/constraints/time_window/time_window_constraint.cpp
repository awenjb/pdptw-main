#include "time_window_constraint.h"
#include "input/data.h"
#include "input/pdptw_data.h"
#include "input/time_window.h"
#include "lns/solution/solution.h"


TimeWindowConstraint::TimeWindowConstraint(Solution const &solution) : Constraint(solution)
{
    // Empty
}

std::unique_ptr<Constraint> TimeWindowConstraint::clone(Solution const &newOwningSolution) const {
    std::unique_ptr<TimeWindowConstraint> clonePtr = std::make_unique<TimeWindowConstraint>(newOwningSolution);
    clonePtr->allRouteReachTimes = allRouteReachTimes;
    return clonePtr;
}


// Copie le vecteur de reach time de la route concernée
// Insère les positions pickup/delivery
// refait l'ordo sur le nouveau vecteur
bool TimeWindowConstraint::checkInsertion(const PDPTWData& data, const Pair & pair, int routeIndex, int pickupPos, int deliveryPos) const
{
    ReachTimeVector const &reachTimes = allRouteReachTimes.at(routeIndex);

    // COPY route vector
    std::vector<int> route(getSolution().getRoute(routeIndex).getRoute().begin(), getSolution().getRoute(routeIndex).getRoute().end()) ;
    // COPY reachTimes vector
    ReachTimeVector newReachTimes(reachTimes.begin(), reachTimes.end());

    // Insert pickup and delivery
    route.insert(route.begin() + pickupPos, pair.getPickup().getId());
    route.insert(route.begin() + deliveryPos, pair.getDelivery().getId());

    // Compute new reach time
    computeReachTimes(data, route, newReachTimes);

    // Check Time Windows
    for (int i = 0; i < reachTimes.size(); ++i) 
    {
        if (! data.getLocation(route[i]).getTimeWindow().isValid(newReachTimes.at(i)) )
        {
            return false;
        }
    }
    return true;    
}

void TimeWindowConstraint::computeReachTimes(const PDPTWData& data, const std::vector<int> & routeIDs, ReachTimeVector & reachTimes) const
{
    // Adjust the size of reachTimes vector
    reachTimes.resize(routeIDs.size(), 0);
    // Time to the first location
    reachTimes[0] = data.getDepot().getTimeWindow().getStart() + data::TravelTime(data, 0, routeIDs.at(0));
    // Compute other reachTimes (max between arrival and start of the time window)
    for (int i = 1; i < routeIDs.size(); ++i) {
        TimeInteger travelTime = data::TravelTime(data, routeIDs[i - 1], routeIDs[i]);
        TimeInteger serviceTime = data.getLocation(routeIDs[i - 1]).getServiceDuration();
        TimeInteger startTW = data.getLocation(routeIDs[i]).getTimeWindow().getStart();
        reachTimes[i] = std::max(reachTimes[i - 1] + serviceTime + travelTime, startTW);
    }
}


void TimeWindowConstraint::ApplyModif(const PDPTWData& data, const Pair & pair, int routeIndex, int pickupPos, int deliveryPos, bool addPair)
{
    // COPY route vector
    std::vector<int> routeIDs(getSolution().getRoute(routeIndex).getRoute().begin(), getSolution().getRoute(routeIndex).getRoute().end());
    
    // Adjust pickup and delivery
    if (addPair)
    {
        routeIDs.insert(routeIDs.begin() + pickupPos, pair.getPickup().getId());
        routeIDs.insert(routeIDs.begin() + deliveryPos, pair.getDelivery().getId());
    }
    else
    {
        routeIDs.erase(routeIDs.begin() + deliveryPos);  
        routeIDs.erase(routeIDs.begin() + pickupPos); 
    }

    // Adjust the size of reachTimes vector
    allRouteReachTimes[routeIndex].resize(routeIDs.size(), 0);
    // Time to the first location
    allRouteReachTimes[routeIndex][0] = data.getDepot().getTimeWindow().getStart() + data::TravelTime(data, 0, routeIDs.at(0));
    // Compute other reachTimes (max between arrival and start of the time window)
    for (int i = 1; i < routeIDs.size(); ++i) {
        TimeInteger travelTime = data::TravelTime(data, routeIDs[i - 1], routeIDs[i]);
        TimeInteger serviceTime = data.getLocation(routeIDs[i - 1]).getServiceDuration();
        TimeInteger startTW = data.getLocation(routeIDs[i]).getTimeWindow().getStart();
        allRouteReachTimes[routeIndex][i] = std::max(allRouteReachTimes[routeIndex][i - 1] + serviceTime + travelTime, startTW);
    }
}


bool TimeWindowConstraint::check(InsertPair const &op) const
{
    return checkInsertion(getSolution().getData(), op.getPair(), op.getRouteIndex(), op.getPickupInsertion(), op.getDeliveryInsertion()); 
}
void TimeWindowConstraint::apply(InsertPair const &op)
{
    ApplyModif(getSolution().getData(), op.getPair(), op.getRouteIndex(), op.getPickupInsertion(), op.getDeliveryInsertion(), true); 
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
    ApplyModif(getSolution().getData(), op.getPair(), op.getRouteIndex(), op.getPickupDeletion(), op.getDeliveryDeletion(), false); 
}


bool TimeWindowConstraint::check(RemoveRoute const &op) const
{
    return true;
}
void TimeWindowConstraint::apply(RemoveRoute const &op)
{
    allRouteReachTimes.erase(allRouteReachTimes.begin() + op.getRouteIndex());
}




