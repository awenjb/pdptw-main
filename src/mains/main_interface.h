#pragma once

#include "input/pdptw_data.h"
#include "lns/operators/reconstruction/list_heuristic_cost_oriented.h"
#include "lns/operators/selector/operator_selector.h"
#include "lns/solution/solution.h"

#include <functional>


/**
 * Function adding multiple reconstruction operators to an operator selector.
 */
void addAllReconstructor(SimpleOperatorSelector &selector);

int mainInterface(int argc, char **argv, std::function<void(PDPTWData &, Solution &)> function);
