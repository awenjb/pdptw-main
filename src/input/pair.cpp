#include "pair.h"

Pair::Pair(const Location& pickupLoc, const Location& deliveryLoc)
: pickup(pickupLoc), delivery(deliveryLoc) {}

const Location& Pair::getPickup() const 
{
    return pickup.get();
}

const Location& Pair::getDelivery() const 
{
    return delivery.get();
}