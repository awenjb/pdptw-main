#pragma once

#include "atomic_modification.h"

class Location;

/**
 * Abstract specification of AtomicModification for modifications that recreate the solution.
 */
class AtomicRecreation : public AtomicModification
{
public:

    /**
     * @return the pickup location added to the solution, nullptr if none were added
     */
    virtual Location const *getAddedLocation() const = 0;
};