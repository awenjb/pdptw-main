
#include "list_heuristic_insertion.h"
#include "lns/operators/sorting_strategy.h"
#include "enumerate.h"


#include <concepts>


ListHeuristicInsertion::ListHeuristicInsertion(SortingStrategyType strategy, EnumerationType enumeration)
    : strategy(strategy), enumeration(enumeration)
{}

void ListHeuristicInsertion::reconstructSolution(Solution &solution, double blinkRate) const
{
    std::vector<int> sortedPairs;
    // selection strategy
    switch (strategy) {
        case SortingStrategyType::SHUFFLE: 
        {
            std::cout << " \n(Shuffle)\n";
            // copy
            sortedPairs = sorting_strategy::Shuffle(solution).sortPairs();
            break;
        }
        default:
            spdlog::error("Error, invalid strategy selected.");
            throw std::invalid_argument("Invalid sorting strategy selected.");
            break;
    }

    AtomicRecreationPtr recreation;
    for (int pairID: sortedPairs)
    {
        Pair const &pair = solution.getData().getPair(pairID);
        recreation = ListHeuristicInsertion::selectRecreation(solution, pair, blinkRate, enumeration);
        if (recreation)
        {
            std::cout << "\n --- Apply recreation --- \n";
            solution.applyRecreateSolution(*recreation);
        }
    }
}

std::unique_ptr<AtomicRecreation> ListHeuristicInsertion::selectRecreation(Solution &solution, Pair const &pair,
                                                                           double blinkRate, EnumerationType enumeration)
{
    AtomicRecreationPtr bestInsertion;
    double bestKnownInsertionCost = std::numeric_limits<double>::max();

    enumeration::ModificationContainer modifications;

    // Enumeration strategy
    switch (enumeration) {
        case EnumerationType::ALL_INSERT_PAIR: 
        {
            std::cout << " \n(All insert pair) \n";
            enumeration::enumerateAllInsertPair(solution, pair, addToListIfValidTemplate<InsertPair>(solution, modifications));
            
            break;
        }
        default:
            spdlog::error("Error, invalid enumeration selected.");
            throw std::invalid_argument("Invalid enumeration strategy selected.");
            break;
    }

    // Keep the modification with the best cost
    for (AtomicRecreationPtr &possibleRecreation: modifications)
    {
        if (util::getRandom() <= 1 - blinkRate)
        {
            double newInsertionCost = possibleRecreation->evaluate(solution);
            if (newInsertionCost < bestKnownInsertionCost)
            {
                bestKnownInsertionCost = newInsertionCost;
                bestInsertion = std::move(possibleRecreation);
            }
        }
    }
    return bestInsertion;
}