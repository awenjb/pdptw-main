#pragma once

#include "atomic_modification.h"
#include <vector>

class Location;

/**
 * Abstract Modification that destruct the solution.
*/
class AtomicDestruction : public AtomicModification
{
public:
    /**
     * @return the location ID removed from the solution.
     */
    virtual std::vector<int> const &getDeletedRequests() const = 0;
};
