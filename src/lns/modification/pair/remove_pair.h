#pragma once

#include <functional>

#include "lns/modification/atomic_destruction.h"
#include "lns/solution/solution.h"
#include "input/location.h"
#include "input/pair.h"
#include "types.h"

/**
 * A modification that will remove a pair (pickup/delivery) of location from the solution.
 */
class RemovePair : public AtomicDestruction
{
    /**
     * The route index on which the deletion will be made
     */
    int routeIndex;

    /**
     * Index at which the deletion must be made
     */
    int pickupDeletion;

    /**
     * Index at which the deletion must be made
     */
    int deliveryDeletion;
   
    /**
     * The pickup location to remove
     */
    Location const & pickupLocation;
    
    /**
     * The delivery location to remove
     */
    Location const & deliveryLocation;

    Pair const & pair;

    /**
     * Removed Location ID (empty before ModifySolution is called)
     */
    std::vector<int> removedLocationID;

public:
    
    RemovePair(int routeIndex, int pickupDeletion, int deliveryDeletion, Pair const &pair);
    RemovePair(Index position, Pair const &pair);
    ~RemovePair() override = default;

    void modifySolution(Solution &solution) override;
    double evaluate(Solution const &solution) const override;
    /**
     *  Return the location ID of location that has been deleted
     */
    std::vector<int> const &getDeletedRequests() const override;

    int getPickupDeletion() const;
    int getDeliveryDeletion() const;
    int getRouteIndex() const;
    Location const &getPickupLocation() const;
    Location const &getDeliveryLocation() const;
    Pair const &getPair() const;
    Index getIndex() const;

};  

