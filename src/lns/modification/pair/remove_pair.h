#pragma once

#include "input/location.h"
#include "input/pair.h"
#include "lns/modification/atomic_destruction.h"
#include "lns/solution/solution.h"
#include "types.h"

#include <functional>
#include <vector>

/**
 * A modification representing the removal of a pickup-delivery pair from a route.
 * 
 * The class stores the route index and the positions of the pickup and delivery locations
 * to be removed. Once applied, it can track which pair was removed.
 */
class RemovePair : public AtomicDestruction
{
    int routeIndex;      // Index of the route from which the pair is removed
    int pickupDeletion;  // Index of the pickup location in the route (pre-modification)
    int deliveryDeletion;// Index of the delivery location in the route (pre-modification)

    Location const &pickupLocation;  // Reference to the pickup location to remove
    Location const &deliveryLocation;//  Reference to the delivery location to remove

    Pair const &pair;// Reference to the full pickup-delivery pair

    /**
     * Removed PairID (= pickupID) (empty before ModifySolution is called)
     */
    std::vector<int> removedPairID;

public:
    RemovePair(int routeIndex, int pickupDeletion, int deliveryDeletion, Pair const &pair);
    RemovePair(Index position, Pair const &pair);
    ~RemovePair() override = default;

    ModificationApplyVariant asApplyVariant() const override;

    void modifySolution(Solution &solution) override;
    double evaluate(Solution const &solution) const override;
    /**
     *  Returns the list of pickup IDs that have been removed.
     */
    std::vector<int> const &getDeletedPairs() const override;

    int getPickupDeletion() const;
    int getDeliveryDeletion() const;
    int getRouteIndex() const;
    Location const &getPickupLocation() const;
    Location const &getDeliveryLocation() const;
    Pair const &getPair() const;
    Index getIndex() const;
};
