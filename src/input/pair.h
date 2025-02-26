#pragma once

#include "location.h"

// Represent a pair pickup/delivery of location
class Pair
{
private:
    std::reference_wrapper<Location const> pickup;
    std::reference_wrapper<Location const> delivery;

public:
    Pair(const Location& pickupLoc, const Location& deliveryLoc);

    const Location& getPickup() const;
    const Location& getDelivery() const;
};