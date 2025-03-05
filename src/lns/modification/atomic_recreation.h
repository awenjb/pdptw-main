#pragma once

#include "atomic_modification.h"
#include "input/pair.h"

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
     * @return the pair added to the solution, nullptr if none were added
     * (why return pointer and not juste a int ?)
     */
    virtual Pair const *getAddedPairs() const = 0;
};