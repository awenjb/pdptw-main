#include "fleet_minimization.h"

#include "config.h"
#include "lns/modification/route/remove_route.h"
#include "lns/operators/destruction/bank_focus_string_removal/bank_focus_string_removal.h"
#include "lns/operators/destruction/clean_empty_route.h"
#include "lns/operators/destruction/split_string_removal.h"
#include "lns/operators/selector/operator_selector.h"
#include "lns_runtime_data.h"
#include "lns_utils.h"
#include "mains/main_interface.h"
#include "output/solution_checker.h"

#include <cmath>
#include <spdlog/spdlog.h>
#include <vector>

int sumAbs(Solution const &solution, std::vector<int> const &absCounter)
{
    int sum = 0;
    for (int request: solution.getBank())
    {
        sum += absCounter.at(request);
    }
    return sum;
}

void removeOneRoute(Solution &solution, std::vector<int> const &absCounter)
{
    int bestRouteIndex = -1;
    int minSum = std::numeric_limits<int>::max();

    auto const &routes = solution.getRoutes();
    for (size_t i = 0; i < routes.size(); ++i)
    {
        int routeSum = 0;
        for (int id: routes.at(i).getRoute())
        {
            routeSum += absCounter.at(id);
        }

        if (routeSum < minSum)
        {
            minSum = routeSum;
            bestRouteIndex = static_cast<int>(i);
        }
    }

    if (bestRouteIndex != -1)
    {
        RemoveRoute remove(bestRouteIndex);
        solution.applyDestructSolution(remove);
    }
}

void fleetMinimizationCVB(LnsRuntimeData &runtime, Solution &currentSolution)
{
    unsigned long startTime = getTimeSinceInSec(runtime.start);
    unsigned long firstPhaseThreshold = MAX_DURATION_SEC * FIRST_PHASE_TIME_THRESHOLD;

    int iterationsWithoutImprovement = 0;

    // Initialize operator selector with destruction and reconstruction strategies
    SimpleOperatorSelector minimizationSelector;
    addAllReconstructor(minimizationSelector);
    minimizationSelector.addDestructor(StringRemoval(10, 10));
    minimizationSelector.addDestructor(SplitStringRemoval(10, 10));

    // Absence counter: how often a request is not served in candidate solutions
    std::vector<int> absCounter = std::vector<int>(currentSolution.getData().getSize() + 1, 0);

    // while (iterationMax > NUMBER_ITERATION * (1-FIRST_PHASE_TIME_THRESHOLD) && iterationsWithoutImprovement < FIRST_PHASE_ITERATION_THRESHOLD)
    while ((getTimeSinceInSec(runtime.start) - startTime) < firstPhaseThreshold &&
           iterationsWithoutImprovement < FIRST_PHASE_ITERATION_THRESHOLD)
    {
        ++runtime.numberOfIteration;
        logProgress(runtime, currentSolution);

        Solution candidateSolution = currentSolution;

        // Choose and apply operator pair
        auto destructReconstructPair = minimizationSelector.getOperatorPair();
        destructReconstructPair.destructor().destroySolution(candidateSolution);
        destructReconstructPair.reconstructor().reconstructSolution(candidateSolution, 0.01);
        candidateSolution.computeAndStoreSolutionCost();

        std::vector<int> const &candidateBank = candidateSolution.getBank();
        std::vector<int> const &currentBank = currentSolution.getBank();

        int currentSumAbs = sumAbs(currentSolution, absCounter);
        int candidateSumAbs = sumAbs(candidateSolution, absCounter);

        bool hasImproved = false;

        // Check if candidate is better
        if ((candidateBank.size() < currentBank.size()) || (candidateSumAbs < currentSumAbs))
        {
            currentSolution = candidateSolution;
            hasImproved = true;
        }

        // If candidate bank is empty, update best solution
        if (candidateBank.empty())
        {
            CleanEmptyRoute clean;
            clean.destroySolution(candidateSolution);

            if (candidateSolution.getRoutes().size() < runtime.bestSolution.getRoutes().size())
            {
                checker::checkAll(candidateSolution, candidateSolution.getData(), false);

                unsigned long now = getTimeSinceInMs(runtime.start);
                updateBestSolution(runtime, candidateSolution, now);

                runtime.bestIterationFleet = runtime.numberOfIteration;
                runtime.bestTimeFleet = getTimeSinceInSec(runtime.start);

                spdlog::info("New Best | Iteration {} \t | Time {}ms \t | Routes {} \t | Cost {}",
                             runtime.numberOfIteration,
                             now,
                             runtime.bestSolution.getRoutes().size(),
                             std::ceil(runtime.bestSolution.getRawCost() * 100.0) / 100.0);
            }

            currentSolution = candidateSolution;
            removeOneRoute(currentSolution, absCounter);
        }

        // Update the number of iterations since last improvement
        iterationsWithoutImprovement = hasImproved ? 0 : iterationsWithoutImprovement + 1;

        // Update absCounter
        for (int request: candidateBank)
        {
            ++absCounter.at(request);
        }
    }
}