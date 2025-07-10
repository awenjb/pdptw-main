#include "lns.h"

#include "config.h"
#include "lns/acceptance/acceptance_function.h"
#include "lns/fleet_minimization.h"
#include "lns/lns_runtime_data.h"
#include "lns/lns_utils.h"
#include "lns/operators/destruction/bank_focus_string_removal/bank_focus_string_removal.h"
#include "lns/operators/destruction/clean_empty_route.h"
#include "lns/operators/selector/operator_selector.h"
#include "mains/main_interface.h"
#include "output/solution_checker.h"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <iterator>
#include <vector>

/**
 * LNS code, based on the initial 2e-VRP framework
 */
output::LnsOutput lns::runLns(Solution const &initialSolution, OperatorSelector &opSelector,
                              AcceptanceFunction const &acceptFunctor)
{
    /**
     * The solution is the base solution used to create the neighbor solution.
     * It is constant unless we accept the candidate solution.
     */
    Solution actualSolution = initialSolution;
    LnsRuntimeData runtime = LnsRuntimeData(actualSolution);

    // fixed iteration
    int iterationMax = NUMBER_ITERATION;

    unsigned long startTime = getTimeSinceInSec(runtime.start);
    unsigned long currentTime = startTime;

    if (TWO_PHASE_ALGORITHM)
    {
        spdlog::info("Route Minimization");
        fleetMinimizationCVB(runtime, actualSolution);
    }

    actualSolution = runtime.bestSolution;
    runtime.transitionTime = getTimeSinceInSec(runtime.start);
    runtime.transitionIteration = runtime.numberOfIteration;

    currentTime = getTimeSinceInSec(runtime.start);
    spdlog::info("SLNS | Iteration {} | Time {}s ", runtime.numberOfIteration, currentTime);

    // while (iterationMax > 0)
    while ((currentTime - startTime) < MAX_DURATION_SEC)
    {
        std::cout << iterationMax << std::endl;
        // Init iteration
        ++runtime.numberOfIteration;
        logProgress(runtime, actualSolution);

        /**
         * The solution on which we apply the operators.
         * It is discarded at the end of each loop if it is not accepted by the Acceptance Function.
         */
        Solution candidateSolution = actualSolution;

        // Chose operator pair
        auto destructReconstructPair = opSelector.getOperatorPair();

        // Apply operators
        destructReconstructPair.destructor().destroySolution(candidateSolution);
        destructReconstructPair.reconstructor().reconstructSolution(candidateSolution, 0.01);
        candidateSolution.computeAndStoreSolutionCost();

        // Update best solution
        if (isBetterSolution(candidateSolution, runtime.bestSolution))
        {
            checker::checkAll(candidateSolution, candidateSolution.getData(), false);


            // remove empty route from the solution
            CleanEmptyRoute clean = CleanEmptyRoute();
            clean.destroySolution(candidateSolution);

            unsigned long now = getTimeSinceInMs(runtime.start);

            runtime.bestSolution = candidateSolution;
            runtime.bestIteration = runtime.numberOfIteration;
            runtime.bestTime = now;

            runtime.bestTimes.emplace_back(now);
            runtime.bestIterations.emplace_back(runtime.numberOfIteration);
            runtime.bestVehicles.emplace_back(runtime.bestSolution.getNumberOfRoutes());
            runtime.bestCosts.emplace_back((runtime.bestSolution.getRawCost() * 100.0) / 100.0);

            opSelector.betterSolutionFound();

            // new best solution !
            spdlog::info("New Best | Iteration {} \t | Time {}ms \t | Routes {} \t | Cost {}",
                         runtime.numberOfIteration,
                         getTimeSinceInMs(runtime.start),
                         runtime.bestSolution.getRoutes().size(),
                         std::ceil(runtime.bestSolution.getRawCost() * 100.0) / 100.0);
        }

        // Check if we use the candidate solution as the new actual solution
        // operator can force to take the new solution
        if (destructReconstructPair.forceTakeSolution() ||
            acceptFunctor(candidateSolution, actualSolution, runtime.bestSolution) == AcceptationStatus::ACCEPT)
        {
            actualSolution = std::move(candidateSolution);
        }

        currentTime = getTimeSinceInSec(runtime.start);
        // --iterationMax;
    }

    spdlog::info("End | Iteration {} \t Time {}s", runtime.numberOfIteration, getTimeSinceInSec(runtime.start));


    auto result = output::LnsOutput(runtime.bestSolution,
                                    runtime.numberOfIteration,
                                    runtime.transitionIteration,
                                    getTimeSinceInSec(runtime.start),
                                    runtime.transitionTime,
                                    runtime.bestIterationFleet,
                                    runtime.bestTimeFleet,
                                    runtime.bestIteration,
                                    runtime.bestTime,
                                    runtime.bestTimes,
                                    runtime.bestIterations,
                                    runtime.bestVehicles,
                                    runtime.bestCosts,
                                    0);


    return result;
}

/**
 * SLNS code
 */
output::LnsOutput lns::runSlns(Solution const &initialSolution, OperatorSelector &opSelectorSmall,
                               OperatorSelector &opSelectorLarge, AcceptanceFunction const &acceptFunctor)
{
    Solution actualSolution = initialSolution;
    LnsRuntimeData runtime = LnsRuntimeData(actualSolution);

    // fixed iteration
    // int iterationMax = NUMBER_ITERATION;

    // Define LNS frequency as proposed by Dumas
    int instanceSize = initialSolution.getData().getSize();
    int frequency = static_cast<int>(pow(std::abs(instanceSize), 1.5));

    unsigned long startTime = getTimeSinceInSec(runtime.start);
    unsigned long currentTime = startTime;

    int SlnsIteration = 0;
    bool largeIteration = false;

    if (TWO_PHASE_ALGORITHM)
    {
        spdlog::info("Route Minimization");
        fleetMinimizationCVB(runtime, actualSolution);
    }

    actualSolution = runtime.bestSolution;
    runtime.transitionTime = getTimeSinceInSec(runtime.start);
    runtime.transitionIteration = runtime.numberOfIteration;

    currentTime = getTimeSinceInSec(runtime.start);

    spdlog::info("SLNS | Iteration {} | Time {}s ", runtime.numberOfIteration, currentTime);

    // while (iterationMax > 0)
    while ((currentTime - startTime) < MAX_DURATION_SEC)
    {
        // Init iteration
        ++runtime.numberOfIteration;
        logProgress(runtime, actualSolution);

        Solution candidateSolution = actualSolution;
        if (SlnsIteration < frequency)
        {
            // Select small operators
            auto destructReconstructPair = opSelectorSmall.getOperatorPair();
            // Apply small operators
            destructReconstructPair.destructor().destroySolution(candidateSolution);
            destructReconstructPair.reconstructor().reconstructSolution(candidateSolution, 0.01);
            candidateSolution.computeAndStoreSolutionCost();

            ++SlnsIteration;
        }
        else
        {
            candidateSolution = runtime.bestSolution;
            // Select large operators
            auto destructReconstructPair = opSelectorLarge.getOperatorPair();
            // Apply large operators
            destructReconstructPair.destructor().destroySolution(candidateSolution);
            destructReconstructPair.reconstructor().reconstructSolution(candidateSolution, 0.01);
            candidateSolution.computeAndStoreSolutionCost();

            largeIteration = true;
            SlnsIteration = 0;
        }

        // New best solution
        if (isBetterSolution(candidateSolution, runtime.bestSolution))
        {
            checker::checkAll(candidateSolution, candidateSolution.getData(), true);

            // remove empty route from the solution
            CleanEmptyRoute clean = CleanEmptyRoute();
            clean.destroySolution(candidateSolution);

            unsigned long now = getTimeSinceInMs(runtime.start);
            updateBestSolution(runtime, candidateSolution, now);// Copy the solution

            spdlog::info("New Best | Iteration {} \t | Time {}ms \t | Routes {} \t | Cost {}",
                         runtime.numberOfIteration,
                         getTimeSinceInMs(runtime.start),
                         runtime.bestSolution.getRoutes().size(),
                         std::ceil(runtime.bestSolution.getRawCost() * 100.0) / 100.0);
        }

        // Check if we accept the candidate solution as the new actual solution
        // A large iteration can force to take the new solution
        if (largeIteration ||
            acceptFunctor(candidateSolution, actualSolution, runtime.bestSolution) == AcceptationStatus::ACCEPT)
        {
            largeIteration = false;
            actualSolution = std::move(candidateSolution);
        }

        currentTime = getTimeSinceInSec(runtime.start);
        // --iterationMax;
    }

    spdlog::info("End | Iteration {} | Time {}s", runtime.numberOfIteration, getTimeSinceInSec(runtime.start));


    // compute total travel time
    if (ELEVATION)
    {
        double travelTime = data::totalTravelTime(runtime.bestSolution.getData(), runtime.bestSolution);

        std::cout << travelTime << std::endl;
    }


    auto result = output::LnsOutput(runtime.bestSolution,
                                    runtime.numberOfIteration,
                                    runtime.transitionIteration,
                                    getTimeSinceInSec(runtime.start),
                                    runtime.transitionTime,
                                    runtime.bestIterationFleet,
                                    runtime.bestTimeFleet,
                                    runtime.bestIteration,
                                    runtime.bestTime,
                                    runtime.bestTimes,
                                    runtime.bestIterations,
                                    runtime.bestVehicles,
                                    runtime.bestCosts,
                                    0);


    return result;
}