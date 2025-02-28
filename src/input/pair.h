#pragma once

#include "location.h"

/**
 *  Represent a pair pickup/delivery of location
 *  The pair ID is the pickup ID
 */ 
class Pair
{
private:
    std::reference_wrapper<Location const> pickup;
    std::reference_wrapper<Location const> delivery;
    int pairID;

public:
    Pair(const Location& pickup, const Location& delivery, int pairID);

    const Location& getPickup() const;
    const Location& getDelivery() const;
    /**
    *   getPickup() but shorter
    */
    const Location& getP() const;
    /**
    *   getDelivery() but shorter
    */
    const Location& getD() const;
    int getID() const;

    void print() const;
};