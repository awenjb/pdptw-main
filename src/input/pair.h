#pragma once

#include "location.h"

/**
 * Represents a pickup/delivery pair of locations.
 * The pair ID is assumed to be the same as the pickup location's ID.
 */
class Pair
{
private:
    std::reference_wrapper<Location const> pickup;  // Reference to the pickup location
    std::reference_wrapper<Location const> delivery;// Reference to the delivery location
    int pairID;                                     // Unique identifier, expected to match the pickup location's ID

public:
    Pair(Location const &pickup, Location const &delivery, int pairID);

    Location const &getPickup() const;
    Location const &getDelivery() const;
    int getID() const;

    void print() const;
};