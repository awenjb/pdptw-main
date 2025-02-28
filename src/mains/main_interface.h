#pragma once

#include "input/pdptw_data.h"
#include "lns/solution/solution.h"

#include <functional>

int mainInterface(int argc, char **argv, std::function<void(PDPTWData &, Solution &)> function);
