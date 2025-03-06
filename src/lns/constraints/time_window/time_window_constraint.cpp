#include "time_window_constraint.h"

#include "input/data.h"
#include "input/pdptw_data.h"
#include "input/time_window.h"
#include "lns/modification/pair/insert_pair.h"
#include "lns/modification/pair/remove_pair.h"
#include "lns/modification/route/remove_route.h"
#include "lns/solution/solution.h"


TimeWindowConstraint::TimeWindowConstraint(const Solution& solution) : Constraint(solution)
{
    allRouteReachTimes.resize(getSolution().getRoutes().size());
}


TimeWindowConstraint::~TimeWindowConstraint() 
{
    allRouteReachTimes.clear();
}

std::unique_ptr<Constraint> TimeWindowConstraint::clone(Solution const &newOwningSolution) const {
    std::unique_ptr<TimeWindowConstraint> clonePtr = std::make_unique<TimeWindowConstraint>(newOwningSolution);
    clonePtr->allRouteReachTimes = allRouteReachTimes;
    return clonePtr;
}

void TimeWindowConstraint::computeReachTimes(const PDPTWData& data, const std::vector<int> & routeIDs, ReachTimeVector & reachTimes) const
{
    // Adjust the size of reachTimes vector
    reachTimes.resize(routeIDs.size(), 0);
    // Time to the first location
    reachTimes[0] = data.getDepot().getTimeWindow().getStart() + data::TravelTime(data, 0, routeIDs.at(0));
    // Compute other reachTimes (max between arrival and start of the time window + previous service time + travel time)
    for (int i = 1; i < routeIDs.size(); ++i) {
        TimeInteger travelTime = data::TravelTime(data, routeIDs[i - 1], routeIDs[i]);
        // locations are indexed from 0 to n-1,
        TimeInteger serviceTime = data.getLocation(routeIDs[i - 1]).getServiceDuration();
        TimeInteger startTW = data.getLocation(routeIDs[i - 1]).getTimeWindow().getStart();

        reachTimes[i] = std::max(reachTimes[i - 1], startTW) + serviceTime + travelTime;
    }
}

void TimeWindowConstraint::initReachTimes()
{
    allRouteReachTimes = std::vector<ReachTimeVector>();
    int i = 0;
    for (const Route& route : getSolution().getRoutes()) 
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
bool TimeWindowConstraint::checkInsertion(const PDPTWData& data, const Pair & pair, int routeIndex, int pickupPos, int deliveryPos) const
{

    ReachTimeVector const &reachTimes = allRouteReachTimes.at(routeIndex);

    // COPY route vector
    std::vector<int> route(getSolution().getRoute(routeIndex).getRoute().begin(), getSolution().getRoute(routeIndex).getRoute().end()) ;
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
        if (! data.getLocation(route[i]).getTimeWindow().isValid(newReachTimes.at(i)) )
        {
            return false;
        }
    }
    return true;    
}



void TimeWindowConstraint::ApplyModif(const PDPTWData& data, const Pair & pair, int routeIndex, int pickupPos, int deliveryPos, bool addPair)
{
    // COPY route vector
    std::vector<int> routeIDs(getSolution().getRoute(routeIndex).getRoute().begin(), getSolution().getRoute(routeIndex).getRoute().end());
    
    // Adjust pickup and delivery
    if (addPair)
    {
        routeIDs.insert(routeIDs.begin() + deliveryPos, pair.getDelivery().getId());
        routeIDs.insert(routeIDs.begin() + pickupPos, pair.getPickup().getId());
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
        TimeInteger startTW = data.getLocation(routeIDs[i - 1]).getTimeWindow().getStart();
        allRouteReachTimes[routeIndex][i] = std::max(allRouteReachTimes[routeIndex][i - 1], startTW) + serviceTime + travelTime;
    }
}

bool TimeWindowConstraint::check(InsertPair const &op) const
{
    std::cout << " #TW Check";
    return checkInsertion(getSolution().getData(), op.getPair(), op.getRouteIndex(), op.getPickupInsertion(), op.getDeliveryInsertion()); 
}
void TimeWindowConstraint::apply(InsertPair const &op)
{
    std::cout << "-> Apply Modification on Time Windows \n";
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

const std::vector<TimeWindowConstraint::ReachTimeVector>& TimeWindowConstraint::getallRouteReachTimes() const {
    return allRouteReachTimes;
}

void TimeWindowConstraint::print() const
{
    std::cout << "Reach Times : \n";
    int i = 0;
    for (const auto& reachTimes : getallRouteReachTimes())
    {
        //std::cout << reachTimes.size() << ", ";
        std::cout << "#" << i << " : ";
        for (const TimeInteger reachTime : reachTimes)
        {
            std::cout << reachTime << ", ";
        }
        std::cout << "\n ";
        i++;
    }
    std::cout << "\n";
}

