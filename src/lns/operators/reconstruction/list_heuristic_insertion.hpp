#pragma once

#include "list_heuristic_insertion.h"
// This is a header, but it does define templates, so we can't put them in .cpp file
// for forward declaration you can use the .h file, but if you need to use the class, you must include this one instead

#include "lns/operators/generators/modification_generator.h"

#include <concepts>


template<std::derived_from<sorting_strategy::SortingStrategy> Strategy,
         std::derived_from<generator::ModificationGenerator> Generator>
ListHeuristicInsertion<Strategy, Generator>::ListHeuristicInsertion() = default;

template<std::derived_from<sorting_strategy::SortingStrategy> Strategy,
         std::derived_from<generator::ModificationGenerator> Generator>
void ListHeuristicInsertion<Strategy, Generator>::reconstructSolution(Solution &solution, double blinkRate) const
{
    std::vector<int> sortedPairs = Strategy(solution).sortPairs();
    AtomicRecreationPtr recreation;
    for (int pairID: sortedPairs)
    {
        Pair const &pair = solution.getData().getPair(pairID);
        recreation = ListHeuristicInsertion::choosingStrategy(solution, pair, blinkRate);
        if (recreation)
        {
            std::cout << "\n --- Apply recreation --- \n";
            solution.applyRecreateSolution(*recreation);
        }
    }
}


template<std::derived_from<sorting_strategy::SortingStrategy> Strategy,
         std::derived_from<generator::ModificationGenerator> Generator>
std::unique_ptr<AtomicRecreation> ListHeuristicInsertion<Strategy, Generator>::choosingStrategy(Solution &solution,
                                                                                                Pair const &pair,
                                                                                                double blinkRate)
{
    AtomicRecreationPtr bestInsertion;
    double bestKnownInsertionCost = std::numeric_limits<double>::max();
    generator::ModificationContainer modifications;
    Generator().populate(solution, pair, modifications);
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