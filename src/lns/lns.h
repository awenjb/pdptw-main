#pragma once

#include "output/lns_output.h"

class Solution;
class OperatorSelector;
class AcceptanceFunction;

namespace lns
{
    output::LnsOutput runLns(Solution const &initialSolution, OperatorSelector &opSelector,
                             AcceptanceFunction const &acceptFunctor);
    output::LnsOutput runSlns(Solution const &initialSolution, OperatorSelector &opSelectorSmall,
                              OperatorSelector &opSelectorLarge, AcceptanceFunction const &acceptFunctor);
}// namespace lns