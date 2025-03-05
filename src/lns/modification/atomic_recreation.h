#pragma once

#include "atomic_modification.h"

class Location;

/**
 * Abstract specification of AtomicModification for modifications that recreate the solution.
 */
class AtomicRecreation : public AtomicModification
{
public:
    ~AtomicRecreation() override = default;

    /**
     * Visitor pattern double dispatch.
     * Only need to be implemented with `return *this;`
     * Update ModificationCheckVariant alias when adding new recreate modification
     */
    virtual ModificationCheckVariant asCheckVariant() const = 0;
    
    /**
     * @return the pickup location added to the solution, nullptr if none were added
     */
    virtual Location const *getAddedLocation() const = 0;
};