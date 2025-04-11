#include "lns.h"

#include "config.h"
#include "lns/acceptance/acceptance_function.h"
#include "lns/operators/destruction/bank_focus_string_removal/bank_focus_string_removal.h"
#include "lns/operators/destruction/clean_empty_route.h"
#include "lns/operators/selector/operator_selector.h"
#include "mains/main_interface.h"
#include "output/solution_checker.h"

#include <chrono>

namespace
{
    bool isBetterSolution(Solution const &candidateSolution, Solution const &bestKnownSol)
    {
        return candidateSolution.getCost() < bestKnownSol.getCost();
    }

    bool isReducingNbRoutes(Solution const &candidateSolution, Solution const &bestSolution)
    {
        return candidateSolution.getBank().empty() &&
               (candidateSolution.getRoutes().size() < bestSolution.getRoutes().size());
    }

    using lns_clock = std::chrono::high_resolution_clock;
    using lns_time_point = std::chrono::time_point<lns_clock, std::chrono::nanoseconds>;

    struct LnsRuntimeData
    {
        Solution bestSolution;
        unsigned int bestIteration = 0;
        lns_time_point bestTime = lns_clock::now();
        unsigned int numberOfIteration = 0;
        lns_time_point start = lns_clock::now();
    };

    /**
     * @return the number of seconds between point and nœow (the moment the function is called).
     */
    unsigned long getTimeSinceInSec(lns_time_point point)
    {
        return std::chrono::duration_cast<std::chrono::seconds>(lns_clock::now() - point).count();
    }

    unsigned long getTimeSinceInMs(lns_time_point point)
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(lns_clock::now() - point).count();
    }

    bool triggerLogProgress(LnsRuntimeData const &)
    {
        static auto lastTrigger = lns_clock::now();
        if (getTimeSinceInSec(lastTrigger) >= 10)
        {
            lastTrigger = lns_clock::now();
            return true;
        }
        return false;
    }

    void logProgress(LnsRuntimeData const &runtime, Solution const &actualSolution)
    {
        if (triggerLogProgress(runtime))
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
                speedLog = fmt::format("{:.1f} i/s", iterPerSecond);
            }
            std::string missingRequestLog;
            long requestsMissing = runtime.bestSolution.missingPairCount();
            //... log TO DO
        }
    }

    void fleetMinimization(int &iterationMax, LnsRuntimeData &runtime, Solution &actualSolution)
    {
        double firstPhaseIteration = NUMBER_ITERATION * (1 - FIRST_PHASE_ITERATION);

        SimpleOperatorSelector minimizationSelector;
        addAllReconstructor(minimizationSelector);
        minimizationSelector.addDestructor(BankFocusStringRemoval(10, 10));
        //minimizationSelector.addDestructor(StringRemoval(10, 10));

        while (iterationMax > firstPhaseIteration)
        {
            // Init iteration
            ++runtime.numberOfIteration;
            logProgress(runtime, actualSolution);

            Solution candidateSolution = actualSolution;

            // Remove empty route from the solution
            if (candidateSolution.getBank().empty())
            {
                CleanEmptyRoute clean = CleanEmptyRoute();
                clean.destroySolution(candidateSolution);
            }

            // Chose operator pair
            auto destructReconstructPair = minimizationSelector.getOperatorPair();
            // Apply operators
            destructReconstructPair.destructor().destroySolution(candidateSolution);
            destructReconstructPair.reconstructor().reconstructSolution(candidateSolution, 0.01);
            candidateSolution.computeAndStoreSolutionCost();

            if (isReducingNbRoutes(candidateSolution, runtime.bestSolution))
            {
                checker::checkAll(candidateSolution, candidateSolution.getData(), false);

                runtime.bestSolution = candidateSolution;
                runtime.bestIteration = runtime.numberOfIteration;
                runtime.bestTime = lns_clock::now();

                minimizationSelector.betterSolutionFound();

                // new best solution !
                spdlog::info("New Best Solution | Routes {} \t Cost {} \t Iteration {} \t Time {}ms",
                             runtime.bestSolution.getRoutes().size(),
                             std::ceil(runtime.bestSolution.getRawCost() * 100.0) / 100.0,
                             runtime.numberOfIteration,
                             getTimeSinceInMs(runtime.start));
            }
            
            // candidateSolution.print();
            // spdlog::info("Actual Solution | Routes {} \t Cost {}",
            //              candidateSolution.getRoutes().size(),
            //              std::ceil(candidateSolution.getRawCost() * 100.0) / 100.0);

            actualSolution = std::move(candidateSolution);

            --iterationMax;

        }
    }


}// namespace

output::LnsOutput lns::runLns(Solution const &initialSolution, OperatorSelector &opSelector,
                              AcceptanceFunction const &acceptFunctor)
{
    /**
     * The solution is the base solution used to create the neighbor solution.
     * It is constant unless we accept the candidate solution.
     */
    Solution actualSolution = initialSolution;
    LnsRuntimeData runtime = {actualSolution};

    // fixed iteration
    int iterationMax = NUMBER_ITERATION;

    if (TWO_PHASE_ALGORITHM)
    {
        spdlog::info("Route Minimization | Iteration {}", NUMBER_ITERATION * FIRST_PHASE_ITERATION);
        fleetMinimization(iterationMax, runtime, actualSolution);
    }

    actualSolution = runtime.bestSolution;

    spdlog::info("SLNS | Iteration {}", NUMBER_ITERATION - NUMBER_ITERATION * FIRST_PHASE_ITERATION);
    while (iterationMax > 0)
    {
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

            runtime.bestSolution = candidateSolution;
            runtime.bestIteration = runtime.numberOfIteration;
            runtime.bestTime = lns_clock::now();
            opSelector.betterSolutionFound();

            // new best solution !
            spdlog::info("New Best Solution | Routes {} \t Cost {} \t Iteration {} \t Time {}ms",
                         runtime.bestSolution.getRoutes().size(),
                         std::ceil(runtime.bestSolution.getRawCost() * 100.0) / 100.0,
                         runtime.numberOfIteration,
                         getTimeSinceInMs(runtime.start));

            //runtime.bestSolution.print();
        }

        // Check if we use the candidate solution as the new actual solution
        // operator can force to take the new solution
        if (destructReconstructPair.forceTakeSolution() ||
            acceptFunctor(candidateSolution, actualSolution, runtime.bestSolution) == AcceptationStatus::ACCEPT)
        {
            actualSolution = std::move(candidateSolution);
        }
        --iterationMax;
    }

    spdlog::info("End | Iteration {} \t Time {}s",
        runtime.numberOfIteration,
        getTimeSinceInSec(runtime.start));

    auto result = output::LnsOutput(
            std::move(runtime.bestSolution), runtime.numberOfIteration, getTimeSinceInSec(runtime.start));
    return result;
}