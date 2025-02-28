#include "pair.h"

Pair::Pair(const Location& pickup, const Location& delivery, int pairID)
: pickup(pickup), delivery(delivery), pairID(pairID) {}

const Location& Pair::getPickup() const 
{
    return pickup.get();
}

const Location& Pair::getDelivery() const 
{
    return delivery.get();
}

const Location& Pair::getP() const 
{
    return pickup.get();
}

const Location& Pair::getD() const 
{
    return delivery.get();
}

int Pair::getID() const 
{
    return pairID;
}

void Pair::print() const
{
    pickup.get().print();
    std::cout << "\n";
    delivery.get().print();
}