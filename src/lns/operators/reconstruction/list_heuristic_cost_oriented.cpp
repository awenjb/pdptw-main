#include "list_heuristic_cost_oriented.h"

void ListHeuristicCostOriented::reconstructSolution(Solution &solution, double blinkRate, SortingStrategyType strategy,
                                                    EnumerationType enumeration) const
{
    std::vector<int> sortedPairs;
    // selection strategy
    switch (strategy)
    {
        case SortingStrategyType::SHUFFLE: {
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

    for (int pairID: sortedPairs)
    {
        Pair const &pair = solution.getData().getPair(pairID);

        AtomicRecreationPtr bestRecreation;
        double bestRecreationCost = std::numeric_limits<double>::max();

        // Enumeration strategy
        switch (enumeration)
        {
            case EnumerationType::ALL_INSERT_PAIR: {
                std::cout << " \n(All insert pair) \n";
                enumeration::enumerateAllInsertPair(
                        solution,
                        pair,
                        keepBestSolution<InsertPair>(solution, bestRecreation, bestRecreationCost, blinkRate));

                break;
            }
            default:
                spdlog::error("Error, invalid enumeration selected.");
                throw std::invalid_argument("Invalid enumeration strategy selected.");
                break;
        }

        if (bestRecreation)
        {
            std::cout << "\n --- Apply recreation --- \n";
            solution.applyRecreateSolution(*bestRecreation);
        }
    }
}