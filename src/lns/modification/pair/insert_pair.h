#pragma once

#include "input/location.h"
#include "input/pair.h"
#include "lns/modification/atomic_recreation.h"
#include "lns/solution/solution.h"
#include "types.h"

#include <functional>
#include <tuple>

/**
 * InsertPair represents a modification that inserts a pair of locations
 * (pickup and delivery) into a specific route at specified positions.
 * 
 * The indices provided refer to the positions in the route before the insertion is performed.
 * - `pickupInsertion` is the index where the pickup will be inserted.
 * - `deliveryInsertion` is the index where the delivery will be inserted.
 */
class InsertPair : public AtomicRecreation
{
    int routeIndex;       // Index of the route where the pair will be inserted.
    int pickupInsertion;  // Index at which the pickup location will be inserted (before modification).
    int deliveryInsertion;// Index at which the delivery location will be inserted (before modification).

    Location const &pickupLocation;  // Reference to the pickup location of the pair.
    Location const &deliveryLocation;// Reference to the delivery location of the pair.

    Pair const &pair;// Reference to the pair being inserted.

public:
    InsertPair(int routeIndex, int pickupInsertion, int deliveryInsertion, Pair const &pair);
    InsertPair(Index position, Pair const &pair);

    ~InsertPair() override = default;

    ModificationApplyVariant asApplyVariant() const override;

    void modifySolution(Solution &solution) override;
    double evaluate(Solution const &solution) const override;
    double getPickupCost(Solution const &solution) const;
    double getDeliveryCost(Solution const &solution) const;
    int getAddedPairs() const override;

    int getPickupInsertion() const;
    int getDeliveryInsertion() const;
    int getRouteIndex() const;
    Location const &getPickupLocation() const;
    Location const &getDeliveryLocation() const;
    Pair const &getPair() const;
    Index getIndex() const;

    ModificationCheckVariant asCheckVariant() const override;
};