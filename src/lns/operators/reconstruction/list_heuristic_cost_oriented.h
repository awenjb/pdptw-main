#pragma once

#include "input/data.h"
#include "input/pdptw_data.h"
#include "lns/operators/abstract_operator.h"
#include "lns/operators/sorting_strategy.h"
#include "utils.h"

/**
 * List heuristic operator but evaluates costs before trying to know if the modification is feasible
 */
class ListHeuristicCostOriented : public ReconstructionOperator
{
public:
    using AtomicRecreationPtr = std::unique_ptr<AtomicRecreation>;
    void reconstructSolution(Solution &solution, double blinkRate, SortingStrategyType strategy,
                             EnumerationType enumeration) const override;
};

/**
 * Create a function that will consume all the enumerated modifications
 * @tparam ModificationType The type of modification enumerated
 * @param bestModificationPtr the pointer to the actual best modification found yet
 * @param bestCost the cost of the best modification
 * @param blinkRate blinking rate
 */
template<std::derived_from<AtomicRecreation> ModificationType>
std::function<void(ModificationType &&)> keepBestSolution(Solution const &solution,
                                                          std::unique_ptr<AtomicRecreation> &bestModificationPtr,
                                                          double &bestCost, double blinkRate)
{
    return [&](ModificationType &&modification) {
        double cost = modification.evaluate(solution);
        // first test the cost
        // if the modification is better, then blink,
        // then check the modification
        // then store the best cost and the modification to the pointer
        if (cost < bestCost && util::getRandom() >= blinkRate && solution.checkModification(modification))
        {
            std::cout << " => Better Modification, update pointer"
                      << "\n";
            bestModificationPtr = std::make_unique<ModificationType>(modification);
            bestCost = cost;
        }
    };
}
