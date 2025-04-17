#pragma once

#include "lns_runtime_data.h"

unsigned long getTimeSinceInSec(LnsRuntimeData::lns_time_point point);
unsigned long getTimeSinceInMs(LnsRuntimeData::lns_time_point point);
void logProgress(const LnsRuntimeData& runtime, const Solution& actualSolution);
bool isReducingNbRoutes(const Solution& candidateSolution, const Solution& bestSolution);
bool isBetterSolution(const Solution& candidateSolution, const Solution& bestKnownSol);
void updateBestSolution(LnsRuntimeData& runtime, const Solution& candidateSolution, unsigned long now);