#pragma once

#include "output/lns_output.h"

class Solution;
class OperatorSelector;
class AcceptanceFunction;

namespace lns
{
    /**
     * @param initialSolution must be a valid solution !
     */
    output::LnsOutput runLns(Solution const &initialSolution, OperatorSelector &opSelector, AcceptanceFunction const &acceptFunctor);
}// namespace lns