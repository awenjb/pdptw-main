#pragma once

#include "atomic_modification.h"


class Location;

/**
 * Abstract Modification that destruct the solution.
*/
class AtomicDestruction : public AtomicModification
{
public:
    ~AtomicDestruction() override = default;
    /**
     * @return the location ID removed from the solution.
     */
    virtual std::vector<int> const &getDeletedPairs() const = 0;
};
