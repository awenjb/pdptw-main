#include "list_heuristic_cost_oriented.h"

ListHeuristicCostOriented::ListHeuristicCostOriented(SortingStrategyType strategy, EnumerationType enumeration)
    : strategy(strategy), enumeration(enumeration)
{}

void ListHeuristicCostOriented::reconstructSolution(Solution &solution, double blinkRate) const
{
    std::vector<int> sortedPairs;
    // selection strategy
    switch (strategy)
    {
        case SortingStrategyType::SHUFFLE: {
            sortedPairs = sorting_strategy::Shuffle(solution).sortPairs();
            break;
        }
        case SortingStrategyType::DEMAND: {
            sortedPairs = sorting_strategy::Demand(solution).sortPairs();
            break;
        }
        case SortingStrategyType::CLOSE: {
            sortedPairs = sorting_strategy::Close(solution).sortPairs();
            break;
        }
        case SortingStrategyType::FAR: {
            sortedPairs = sorting_strategy::Far(solution).sortPairs();
            break;
        }
        case SortingStrategyType::TWWIDTH: {
            sortedPairs = sorting_strategy::TimeWindowWidth(solution).sortPairs();
            break;
        }
        case SortingStrategyType::TWSTART: {
            sortedPairs = sorting_strategy::TimeWindowStart(solution).sortPairs();
            break;
        }
        case SortingStrategyType::TWEND: {
            sortedPairs = sorting_strategy::TimeWindowEnd(solution).sortPairs();
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
            solution.applyRecreateSolution(*bestRecreation);
        }
    }
}