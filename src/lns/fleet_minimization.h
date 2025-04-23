#pragma once

#include "lns_runtime_data.h"

void fleetMinimization(int& iterationMax, LnsRuntimeData& runtime, Solution& actualSolution);

void fleetMinimizationCVB(/*int& iterationMax,*/ LnsRuntimeData& runtime, Solution& actualSolution);

int sumAbs(Solution const &solution, std::vector<int> const &absCounter);

void removeOneRoute(Solution &solution, std::vector<int> const &absCounter);

