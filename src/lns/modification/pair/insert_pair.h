#pragma once

#include <functional>
#include <tuple>

#include "lns/modification/atomic_recreation.h"
#include "lns/solution/solution.h"
#include "input/location.h"
#include "input/pair.h"
#include "types.h"


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
    
    InsertPair(int routeIndex, int pickupInsertion, int deliveryInsertion, Pair const &pair);
    InsertPair(Index position, Pair const &pair);

    ~InsertPair() override = default;

    void modifySolution(Solution &solution) override;
    double evaluate(Solution const &solution) const override;
    Location const *getAddedLocation() const override;

    int getPickupInsertion() const;
    int getDeliveryInsertion() const;
    int getRouteIndex() const;
    Location const &getPickupLocation() const;
    Location const &getDeliveryLocation() const;
    Pair const &getPair() const;
    Index getIndex() const;

};  