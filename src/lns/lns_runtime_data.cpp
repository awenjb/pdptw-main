#include "lns_runtime_data.h"
#include "config.h"

LnsRuntimeData::LnsRuntimeData(Solution sol) : bestSolution(sol)
{
    const size_t reserveSize = NUMBER_ITERATION * 0.1;
    bestTimes.reserve(reserveSize);
    bestIterations.reserve(reserveSize);
    bestVehicles.reserve(reserveSize);
    bestCosts.reserve(reserveSize);
}
