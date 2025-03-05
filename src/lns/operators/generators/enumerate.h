#pragma once

#include "lns/modification/pair/insert_pair.h"

#include <functional>

namespace enumeration
{

    /**
     * Enumerate InsertDelivery modifications.
     * consumeModification is called for each modification.
     * Does some checks to cut some loops.
     * @param solution
     * @param request
     * @param consumeModification called when a modification is created (ex : keepBestSolution)
     */
    void enumerateAllInsertPair(Solution const &solution, Pair const &Pair,
                                    std::function<void(InsertPair &&)> const &consumeModification);


}// namespace enumeration

