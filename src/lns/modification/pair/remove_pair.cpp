#include "remove_pair.h"
#include "input/data.h"


RemovePair::RemovePair(int routeIndex, int pickupDeletion, int deliveryDeletion, Pair const &pair) :
    routeIndex(routeIndex), 
    pickupDeletion(pickupDeletion), 
    deliveryDeletion(deliveryDeletion), 
    pickupLocation(pair.getPickup()), 
    deliveryLocation(pair.getDelivery()),
    pair(pair) {}


RemovePair::RemovePair(Index position, Pair const &pair) :
    routeIndex(std::get<0>(position)), 
    pickupDeletion(std::get<1>(position)), 
    deliveryDeletion(std::get<2>(position)), 
    pickupLocation(pair.getPickup()), 
    deliveryLocation(pair.getDelivery()),
    pair(pair) {}


void RemovePair::modifySolution(Solution &solution)
{
    Route &route = solution.getRoute(routeIndex);

    // update removedLocationID
    removedLocationID.push_back(route.getRoute()[pickupDeletion]);
    removedLocationID.push_back(route.getRoute()[deliveryDeletion]);

    // remove the delivery before (to not have to update the index)
    route.deleteAt(deliveryDeletion);
    route.deleteAt(pickupDeletion);


}

double RemovePair::evaluate(Solution const &solution) const
{
    Route const &route = solution.getRoute(routeIndex);
    const std::vector<int> & routeIDs = route.getRoute();
    const PDPTWData &data = solution.getData();

    int prevPickup = (pickupDeletion == 0) ? 0 : routeIDs[pickupDeletion - 1];
    // pickup location should not be at the end of a route anyway
    int nextPickup = (pickupDeletion >= routeIDs.size()) ? 0 : routeIDs[pickupDeletion + 1];

    double pickupCost = data::removedCostForSuppression(data, prevPickup, pickupLocation.getId(), nextPickup);

    int prevDelivery = (deliveryDeletion == 0) ? 0 : routeIDs[deliveryDeletion - 1]; 
    int nextDelivery = (deliveryDeletion >= routeIDs.size()) ? 0 : routeIDs[deliveryDeletion + 1];
    if (deliveryDeletion == pickupDeletion+1)
    {
        prevDelivery = prevPickup;
    }

    double deliveryCost = data::removedCostForSuppression(data, prevDelivery, deliveryLocation.getId(), nextDelivery);

    return pickupCost + deliveryCost;
}

int RemovePair::getPickupDeletion() const
{
    return pickupDeletion;

}

int RemovePair::getDeliveryDeletion() const
{
    return deliveryDeletion;
}

int RemovePair::getRouteIndex() const
{
    return routeIndex;
}

Location const & RemovePair::getPickupLocation() const
{
    return pickupLocation;
}

Location const & RemovePair::getDeliveryLocation() const
{
    return deliveryLocation;
}

std::vector<int> const & RemovePair::getDeletedRequests() const
{
    return removedLocationID;
}

Pair const &RemovePair::getPair() const
{
    return pair;
}

Index RemovePair::getIndex() const 
{
    return std::make_tuple(routeIndex, pickupDeletion, deliveryDeletion);
}