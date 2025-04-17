#include "lns_utils.h"
#include <spdlog/spdlog.h>
#include <cmath>
#include <fmt/core.h>

unsigned long getTimeSinceInSec(LnsRuntimeData::lns_time_point point)
{
    return std::chrono::duration_cast<std::chrono::seconds>(LnsRuntimeData::lns_clock::now() - point).count();
}

unsigned long getTimeSinceInMs(LnsRuntimeData::lns_time_point point)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(LnsRuntimeData::lns_clock::now() - point).count();
}

bool isBetterSolution(const Solution& candidateSolution, const Solution& bestKnownSol)
{
    return candidateSolution.getCost() < bestKnownSol.getCost();
}

bool isReducingNbRoutes(const Solution& candidateSolution, const Solution& bestSolution)
{
    return candidateSolution.getBank().empty() &&
           (candidateSolution.getRoutes().size() < bestSolution.getRoutes().size());
}

static bool triggerLogProgress()
{
    static auto lastTrigger = LnsRuntimeData::lns_clock::now();
    if (getTimeSinceInSec(lastTrigger) >= 10)
    {
        lastTrigger = LnsRuntimeData::lns_clock::now();
        return true;
    }
    return false;
}

void logProgress(const LnsRuntimeData& runtime, const Solution& actualSolution)
{
    if (triggerLogProgress())
    {
        unsigned long actualTime = getTimeSinceInMs(runtime.start);
        double iterPerSecond = 1000 * runtime.numberOfIteration / static_cast<double>(actualTime);

        std::string speedLog;
        if (iterPerSecond < 1)
        {
            speedLog = fmt::format("{}s/100 iterations", 100 / iterPerSecond);
        }
        else
        {
            speedLog = fmt::format("{:.1f} it/s", iterPerSecond);
        }

        long requestsMissing = runtime.bestSolution.missingPairCount();
        spdlog::info("Progress | Iteration {} \t | Time {}s \t | Speed: {} | Missing requests: {}",
                     runtime.numberOfIteration,
                     actualTime / 1000,
                     speedLog,
                     requestsMissing);
    }
}

void updateBestSolution(LnsRuntimeData& runtime, const Solution& candidateSolution, unsigned long now) {
    runtime.bestSolution = candidateSolution;
    runtime.bestIteration = runtime.numberOfIteration;
    runtime.bestTime = now;

    runtime.bestTimes.emplace_back(now);
    runtime.bestIterations.emplace_back(runtime.numberOfIteration);
    runtime.bestVehicles.emplace_back(runtime.bestSolution.getNumberOfRoutes());
    runtime.bestCosts.emplace_back((runtime.bestSolution.getRawCost() * 100.0) / 100.0);
}