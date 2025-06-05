#pragma once

#include "lns/modification/pair/insert_pair.h"

#include <forward_list>
#include <functional>

/**
 * A type of enumeration for a specific modification
 */
enum class EnumerationType
{
    ALL_INSERT_PAIR
};

namespace enumeration
{
    using ModificationContainer = std::forward_list<std::unique_ptr<AtomicRecreation>>;

    /**
     * Enumerate InsertDelivery modifications.
     * Does some checks to cut some loops.
     * @param solution
     * @param Pair
     * @param list
     */
    void enumerateAllInsertPair(Solution const &solution, Pair const &pair,
                                std::function<void(InsertPair &&)> const &consumeModification);
    void enumerateAllInsertPairOpti(Solution const &solution, Pair const &pair,
                                    std::unique_ptr<AtomicRecreation> &bestModificationPtr, double &bestCost,
                                    double blinkRate);
    void enumerateAllInsertPairLTTKBest(Solution const &solution, Pair const &pair,
                                        std::unique_ptr<AtomicRecreation> &bestModificationPtr, double &bestCost,
                                        double blinkRate, size_t k);
}// namespace enumeration
