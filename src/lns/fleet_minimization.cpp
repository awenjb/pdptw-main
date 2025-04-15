#include "config.h"
#include "fleet_minimization.h"
#include "lns/operators/destruction/bank_focus_string_removal/bank_focus_string_removal.h"
#include "lns/operators/destruction/clean_empty_route.h"
#include "lns/operators/selector/operator_selector.h"
#include "lns_runtime_data.h"
#include "lns_utils.h"
#include "mains/main_interface.h"
#include "output/solution_checker.h"

#include <cmath>
#include <spdlog/spdlog.h>

void fleetMinimization(int &iterationMax, LnsRuntimeData &runtime, Solution &actualSolution)
{
    double firstPhaseIteration = NUMBER_ITERATION * (1 - FIRST_PHASE_ITERATION);

    SimpleOperatorSelector minimizationSelector;
    addAllReconstructor(minimizationSelector);
    minimizationSelector.addDestructor(BankFocusStringRemoval(10, 10));

    while (iterationMax > firstPhaseIteration)
    {
        ++runtime.numberOfIteration;
        logProgress(runtime, actualSolution);

        Solution candidateSolution = actualSolution;

        if (candidateSolution.getBank().empty())
        {
            CleanEmptyRoute clean;
            clean.destroySolution(candidateSolution);
        }

        auto destructReconstructPair = minimizationSelector.getOperatorPair();
        destructReconstructPair.destructor().destroySolution(candidateSolution);
        destructReconstructPair.reconstructor().reconstructSolution(candidateSolution, 0.01);
        candidateSolution.computeAndStoreSolutionCost();

        if (isReducingNbRoutes(candidateSolution, runtime.bestSolution))
        {
            checker::checkAll(candidateSolution, candidateSolution.getData(), false);

            unsigned long now = getTimeSinceInMs(runtime.start);

            runtime.bestSolution = candidateSolution;
            runtime.bestIterationFleet = runtime.numberOfIteration;
            runtime.bestTimeFleet = now;
            runtime.bestIteration = runtime.numberOfIteration;
            runtime.bestTime = now;

            runtime.bestTimes.emplace_back(now);
            runtime.bestIterations.emplace_back(runtime.numberOfIteration);
            runtime.bestVehicles.emplace_back(runtime.bestSolution.getNumberOfRoutes());
            runtime.bestCosts.emplace_back((runtime.bestSolution.getRawCost() * 100.0) / 100.0);

            minimizationSelector.betterSolutionFound();

            spdlog::info("New Best Solution | Routes {} \t Cost {} \t Iteration {} \t Time {}ms",
                         runtime.bestSolution.getRoutes().size(),
                         std::ceil(runtime.bestSolution.getRawCost() * 100.0) / 100.0,
                         runtime.numberOfIteration,
                         getTimeSinceInMs(runtime.start));
        }

        actualSolution = std::move(candidateSolution);
        --iterationMax;
    }
}