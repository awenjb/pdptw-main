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
    int routeIndex = 0;
    int index = 0;

    int min = std::numeric_limits<int>::max();

    std::vector<Route> const &routes = solution.getRoutes();
    for (Route const &route: routes)
    {
        int sum = 0;
        std::vector<int> const &locIDs = route.getRoute();
        for (int id: locIDs)
        {
            sum += absCounter.at(id);
        }
        if (sum < min)
        {
            min = sum;
            routeIndex = index;
        }
        ++index;
    }

    RemoveRoute remove = RemoveRoute(routeIndex);
    solution.applyDestructSolution(remove);
}

void fleetMinimizationCVB(/*int &iterationMax,*/ LnsRuntimeData &runtime, Solution &actualSolution)
{
    // const double firstPhaseThreshold = NUMBER_ITERATION * (1 - FIRST_PHASE_ITERATION);

    unsigned long startTime = getTimeSinceInSec(runtime.start);
    unsigned long currentTime = startTime;
    unsigned long firstPhaseThreshold = MAX_DURATION_SEC * FIRST_PHASE_THRESHOLD;

    SimpleOperatorSelector minimizationSelector;
    //minimizationSelector.addReconstructor(ListHeuristicCostOriented(SortingStrategyType::DEMAND, EnumerationType::ALL_INSERT_PAIR), 1);
    addAllReconstructor(minimizationSelector);
    // minimizationSelector.addDestructor(BankFocusStringRemoval(10, 10));
    // int manyPairs = actualSolution.getData().getSize() * 40 / 100;
    // minimizationSelector.addDestructor(RandomDestroy(manyPairs));

    minimizationSelector.addDestructor(StringRemoval(10, 10));
    minimizationSelector.addDestructor(SplitStringRemoval(10, 10));



    // counter of the number of solutions where c was not served by any routes
    std::vector<int> absCounter = std::vector<int>(actualSolution.getData().getSize() + 1, 0);


    while ((currentTime - startTime) < firstPhaseThreshold)
    {
        ++runtime.numberOfIteration;
        logProgress(runtime, actualSolution);

        Solution candidateSolution = actualSolution;
        auto destructReconstructPair = minimizationSelector.getOperatorPair();

        destructReconstructPair.destructor().destroySolution(candidateSolution);
        destructReconstructPair.reconstructor().reconstructSolution(candidateSolution, 0.01);
        candidateSolution.computeAndStoreSolutionCost();

        std::vector<int> const &candidateBank = candidateSolution.getBank();

        // is Better Candidate
        if ((candidateBank.size() < actualSolution.getBank().size()) ||
            (sumAbs(candidateSolution, absCounter) < sumAbs(actualSolution, absCounter)))
        {
            //std::cout << "better candidate" << std::endl;
            actualSolution = candidateSolution;
        }

        // is Empty Candidate Bank
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

            actualSolution = candidateSolution;
            removeOneRoute(actualSolution, absCounter);
        }

        // update absCounter
        for (int i: candidateBank)
        {
            ++absCounter.at(i);
        }

        currentTime = getTimeSinceInSec(runtime.start);
        // --iterationMax;
    }
}