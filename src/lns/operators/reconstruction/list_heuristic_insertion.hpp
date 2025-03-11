#pragma once

#include "list_heuristic_insertion.h"
#include "lns/operators/sorting_strategy.h"
#include "enumerate.h"
// This is a header, but it does define templates, so we can't put them in .cpp file
// for forward declaration you can use the .h file, but if you need to use the class, you must include this one instead


#include <concepts>


ListHeuristicInsertion::ListHeuristicInsertion() = default;

void ListHeuristicInsertion::reconstructSolution(Solution &solution, double blinkRate, SortingStrategyType strategy, EnumerationType enumeration) const
{
    std::vector<int> sortedPairs;
    // selection strategy
    switch (strategy) {
        case SortingStrategyType::SHUFFLE: 
        {
            std::cout << " \n(Shuffle)\n";
            sortedPairs = sorting_strategy::Shuffle(solution).sortPairs();
            break;
        }
        default:
            spdlog::error("Error, strategy selected.");
            break;
    }

    AtomicRecreationPtr recreation;
    for (int pairID: sortedPairs)
    {
        Pair const &pair = solution.getData().getPair(pairID);
        recreation = ListHeuristicInsertion::choosingStrategy(solution, pair, blinkRate, enumeration);
        if (recreation)
        {
            std::cout << "\n --- Apply recreation --- \n";
            solution.applyRecreateSolution(*recreation);
        }
    }
}

std::unique_ptr<AtomicRecreation> ListHeuristicInsertion::choosingStrategy(Solution &solution, Pair const &pair,
                                                                           double blinkRate, EnumerationType enumeration)
{
    AtomicRecreationPtr bestInsertion;
    double bestKnownInsertionCost = std::numeric_limits<double>::max();

    enumeration::ModificationContainer modifications;
    //Generator().populate(solution, pair, modifications);
    //

    // Enumeration strategy
    switch (enumeration) {
        case EnumerationType::ALL_INSERT_PAIR: 
        {
            std::cout << " \n(All insert pair) \n";
            //enumerateAllInsertPair(Solution const &solution, Pair const &pair, ModificationContainer &list)
            enumeration::enumerateAllInsertPair(solution, pair, modifications);
            break;
        }
        default:
            spdlog::error("Error, enumeration selected.");
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