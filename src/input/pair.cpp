#include "pair.h"

Pair::Pair(Location const &pickup, Location const &delivery, int pairID)
    : pickup(pickup), delivery(delivery), pairID(pairID)
{}

Location const &Pair::getPickup() const
{
    return pickup.get();
}

Location const &Pair::getDelivery() const
{
    return delivery.get();
}

int Pair::getID() const
{
    return pairID;
}

// Display
void Pair::print() const
{
    pickup.get().print();
    std::cout << "\n";
    delivery.get().print();
}