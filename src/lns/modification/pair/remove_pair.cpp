#include "remove_pair.h"

#include "input/data.h"
#include "lns/solution/solution.h"

RemovePair::RemovePair(int routeIndex, int pickupDeletion, int deliveryDeletion, Pair const &pair)
    : routeIndex(routeIndex), pickupDeletion(pickupDeletion), deliveryDeletion(deliveryDeletion),
      pickupLocation(pair.getPickup()), deliveryLocation(pair.getDelivery()), pair(pair)
{}

RemovePair::RemovePair(Index position, Pair const &pair)
    : routeIndex(std::get<0>(position)), pickupDeletion(std::get<1>(position)), deliveryDeletion(std::get<2>(position)),
      pickupLocation(pair.getPickup()), deliveryLocation(pair.getDelivery()), pair(pair)
{}

void RemovePair::modifySolution(Solution &solution)
{
    Route &route = solution.getRoute(routeIndex);

    // Store the ID of the removed pickup for tracking
    removedPairID.push_back(route.getRoute().at(pickupDeletion));

    // Remove delivery first to keep pickupDeletion index valid
    route.deleteAt(deliveryDeletion);
    route.deleteAt(pickupDeletion);
}

double RemovePair::evaluate(Solution const &solution) const
{
    Route const &route = solution.getRoute(routeIndex);
    std::vector<int> const &routeIDs = route.getRoute();
    PDPTWData const &data = solution.getData();

    // Estimate cost savings from removing the pickup
    int prevPickup = (pickupDeletion == 0) ? 0 : routeIDs.at(pickupDeletion - 1);
    int nextPickup = (pickupDeletion >= routeIDs.size()) ? 0 : routeIDs.at(pickupDeletion + 1);
    double pickupCost = data::removedCostForSuppression(data, prevPickup, pickupLocation.getId(), nextPickup);

    // Estimate cost savings from removing the delivery
    int prevDelivery = (deliveryDeletion == 0) ? 0 : routeIDs.at(deliveryDeletion - 1);
    int nextDelivery = (deliveryDeletion >= routeIDs.size()) ? 0 : routeIDs.at(deliveryDeletion + 1);
    
    // Adjust for consecutive pickup-delivery pair
    if (deliveryDeletion == pickupDeletion + 1)
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

Location const &RemovePair::getPickupLocation() const
{
    return pickupLocation;
}

Location const &RemovePair::getDeliveryLocation() const
{
    return deliveryLocation;
}

std::vector<int> const &RemovePair::getDeletedPairs() const
{
    return removedPairID;
}

Pair const &RemovePair::getPair() const
{
    return pair;
}

Index RemovePair::getIndex() const
{
    return std::make_tuple(routeIndex, pickupDeletion, deliveryDeletion);
}

ModificationApplyVariant RemovePair::asApplyVariant() const
{
    return *this;
}