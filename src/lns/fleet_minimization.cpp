#include "fleet_minimization.h"

#include "config.h"
#include "lns/modification/route/remove_route.h"
#include "lns/operators/destruction/bank_focus_string_removal/bank_focus_string_removal.h"
#include "lns/operators/destruction/clean_empty_route.h"
#include "lns/operators/selector/operator_selector.h"
#include "lns_runtime_data.h"
#include "lns_utils.h"
#include "mains/main_interface.h"
#include "output/solution_checker.h"

#include <cmath>
#include <spdlog/spdlog.h>
#include <vector>

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

            // new best solution !
            spdlog::info("New Best | Iteration {} \t | Time {}ms \t | Routes {} \t | Cost {}",
                         runtime.numberOfIteration,
                         getTimeSinceInMs(runtime.start),
                         runtime.bestSolution.getRoutes().size(),
                         std::ceil(runtime.bestSolution.getRawCost() * 100.0) / 100.0);
        }

        actualSolution = std::move(candidateSolution);
        --iterationMax;
    }
}

int sumAbs(Solution const &solution, std::vector<int> const &absCounter)
{
    int sum = 0;
    for (int i: solution.getBank())
    {
        sum += absCounter.at(i);
    }
    return sum;
}

void removeOneRoute(Solution &solution, std::vector<int> const &absCounter)
{
    //RemoveRoute remove = RemoveRoute(util::getRandomInt(1, solution.getRoutes().size()) - 1);

    int routeIndex = 0;
    int sum = 0;
    int max = 0;
    int index = 0;
    std::vector<Route> routes = solution.getRoutes();
    for (Route const &route : routes)
    {
        std::vector<int> locIDs = route.getRoute();
        for (int id : locIDs)
        {
            sum += absCounter.at(id);
        }
        if (max < sum)
        {
            max = sum;
            routeIndex = index;
        }
        ++index;
    }
    RemoveRoute remove = RemoveRoute(routeIndex);
    solution.applyDestructSolution(remove);
}

void fleetMinimizationCVB(int &iterationMax, LnsRuntimeData &runtime, Solution &actualSolution)
{
    double firstPhaseIteration = NUMBER_ITERATION * (1 - FIRST_PHASE_ITERATION);

    SimpleOperatorSelector minimizationSelector;
    addAllReconstructor(minimizationSelector);
    minimizationSelector.addDestructor(BankFocusStringRemoval(10, 10));
    //minimizationSelector.addDestructor(StringRemoval(10, 10));

    // counter of the number of solutions where c was not served by any routes
    std::vector<int> absCounter = std::vector<int>(actualSolution.getData().getSize() + 1, 0);

    while (iterationMax > firstPhaseIteration)
    {
        ++runtime.numberOfIteration;
        logProgress(runtime, actualSolution);

        Solution candidateSolution = actualSolution;

        auto destructReconstructPair = minimizationSelector.getOperatorPair();
        destructReconstructPair.destructor().destroySolution(candidateSolution);
        destructReconstructPair.reconstructor().reconstructSolution(candidateSolution, 0.01);
        candidateSolution.computeAndStoreSolutionCost();

        std::vector<int> const &candidateBank = candidateSolution.getBank();

        if (candidateBank.empty())
        {
            // clean empty routes
            CleanEmptyRoute clean = CleanEmptyRoute();
            clean.destroySolution(candidateSolution);

            if (candidateSolution.getRoutes().size() <= runtime.bestSolution.getRoutes().size() && candidateSolution.getCost() < runtime.bestSolution.getCost())
            {
                // update best solution
                checker::checkAll(candidateSolution, candidateSolution.getData(), false);

                unsigned long now = getTimeSinceInMs(runtime.start);
                updateBestSolution(runtime, candidateSolution, now);
                runtime.bestIterationFleet = runtime.numberOfIteration;
                runtime.bestTimeFleet = now;

                // new best solution !
                spdlog::info("New Best | Iteration {} \t | Time {}ms \t | Routes {} \t | Cost {}",
                             runtime.numberOfIteration,
                             getTimeSinceInMs(runtime.start),
                             runtime.bestSolution.getRoutes().size(),
                             std::ceil(runtime.bestSolution.getRawCost() * 100.0) / 100.0);
            }

            // remove a route
            removeOneRoute(candidateSolution, absCounter);
        }

        if ((candidateBank.size() < actualSolution.getBank().size()) ||
            (sumAbs(candidateSolution, absCounter) < sumAbs(actualSolution, absCounter)))
        {
            actualSolution = candidateSolution;
        }

        for (int i: candidateBank)
        {
            ++absCounter.at(i);
        }
        --iterationMax;
    }
}