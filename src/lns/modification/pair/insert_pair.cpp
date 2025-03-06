#include "insert_pair.h"
#include "input/data.h"
#include "lns/constraints/constraint.h"


InsertPair::InsertPair(int routeIndex, int pickupInsertion, int deliveryInsertion, Pair const &pair) : 
    routeIndex(routeIndex), 
    pickupInsertion(pickupInsertion), 
    deliveryInsertion(deliveryInsertion), 
    pickupLocation(pair.getPickup()), 
    deliveryLocation(pair.getDelivery()),
    pair(pair) {}

InsertPair::InsertPair(Index position, Pair const &pair) :
    routeIndex(std::get<0>(position)), 
    pickupInsertion(std::get<1>(position)), 
    deliveryInsertion(std::get<2>(position)), 
    pickupLocation(pair.getPickup()), 
    deliveryLocation(pair.getDelivery()),
    pair(pair) {}


void InsertPair::modifySolution(Solution &solution) 
{
    Route &route = solution.getRoute(routeIndex);
    route.insertAt(pickupLocation.getId(), pickupInsertion);
    // + 1 because the pickup location was inserted earlier in the route
    route.insertAt(deliveryLocation.getId(), deliveryInsertion+1);
}

double InsertPair::evaluate(Solution const &solution) const {

    Route const &route = solution.getRoute(routeIndex);
    const std::vector<int> & routeIDs = route.getRoute();
    const PDPTWData &data = solution.getData();

    int prevPickup = (pickupInsertion == 0) ? 0 : routeIDs[pickupInsertion - 1];
    int nextPickup = (pickupInsertion >= routeIDs.size()) ? 0 : routeIDs[pickupInsertion];
    double pickupCost = data::addedCostForInsertion(data, prevPickup, pickupLocation.getId(), nextPickup);


    // if pickupInsertion == deliveryInsertion+1, then prevDelivery = pickupLocation.getId()
    // the insertion of the delivery is done just after the pickup without intermediate location
    // otherwise, the pickup and the delivery insertion are independant and the pickup insertion does not affect the delivery insertion cost

    int prevDelivery = (deliveryInsertion == 0) ? 0 : routeIDs[deliveryInsertion - 1]; 
    if (pickupInsertion == deliveryInsertion)
    {
        prevDelivery = pickupLocation.getId();
    }
    int nextDelivery = (deliveryInsertion >= routeIDs.size()) ? 0 : routeIDs[deliveryInsertion];

    //std::cout << "insert " << prevDelivery << " + " << deliveryLocation.getId() << " + " << nextDelivery << "\n";
    double deliveryCost = data::addedCostForInsertion(data, prevDelivery, deliveryLocation.getId(), nextDelivery);
    
    //std::cout << "insert " << pickupCost << " + " << deliveryCost << "\n";
    return pickupCost + deliveryCost;
}


ModificationCheckVariant InsertPair::asCheckVariant() const
{
    return *this;
}

int InsertPair::getPickupInsertion() const
{
    return pickupInsertion;
}

int InsertPair::getDeliveryInsertion() const
{
    return deliveryInsertion;
}

int InsertPair::getRouteIndex() const
{
    return routeIndex;
}

Location const &InsertPair::getPickupLocation() const 
{
    return pickupLocation;
}

const Location &InsertPair::getDeliveryLocation() const
{
    return deliveryLocation;
}

int InsertPair::getAddedPairs() const 
{
    return pair.getID();
}

const Pair &InsertPair::getPair() const 
{
    return pair;
}

Index InsertPair::getIndex() const 
{
    return std::make_tuple(routeIndex, pickupInsertion, deliveryInsertion);
}

ModificationApplyVariant InsertPair::asApplyVariant() const
{
    return (*this);
}