#pragma once

#include "./../atomic_recreation.h"
#include "./../../solution/solution.h"
#include "./../../../input/location.h"
#include "./../../../input/pair.h"
#include <functional>

class Route;

/**
 * A modification that will insert a pair (pickup/delivery) of location in a route at the given index.
 * First index for the pickup location and second index for the delivery location.
 * Insertion index are the index before modification !
 */
class InsertPair : public AtomicRecreation
{
    /**
     * The route index on which the insertion will be made
     */
    int routeIndex;

    /**
     * Index at which the insertion must be made
     */
    int pickupInsertion;
    /**
     * Index at which the insertion must be made
     */
    int deliveryInsertion;
   
    /**
     * The pickup location to insert
     */
    Location const & pickupLocation;
    /**
     * The delivery location to insert
     */
    Location const & deliveryLocation;

    Pair const & pair;

public:
    InsertPair(int routeIndex, int pickupInsertion, int deliveryInsertion, Location const &pickupLocation, Location const &deliveryLocation);
    InsertPair(int routeIndex, int pickupInsertion, int deliveryInsertion, Pair const &pair);

    void modifySolution(Solution &solution) override;
    double evaluate(Solution const &solution) const override;

    int getPickupInsertion() const;
    int getDeliveryInsertion() const;
    int getRouteIndex() const;
    Location const &getPickupLocation() const;
    Location const &getDeliveryLocation() const;
    Pair const &getPair() const;

    Location const *getAddedLocation() const override;
    
};  