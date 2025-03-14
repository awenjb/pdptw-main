#pragma once

#include "input/data.h"
#include "input/pdptw_data.h"
#include "lns/operators/abstract_operator.h"
#include "lns/operators/sorting_strategy.h"
#include "utils.h"

#include <optional>

class AtomicRecreation;

/**
 * This is a template class for list heuristic operator.
 * Implementation is in the .hpp file
 * @tparam Strategy defines in which order we are treating the requests
 * @tparam Generator defines which modifications are going to be used
 */
class ListHeuristicInsertion : public ReconstructionOperator
{
private:
    using AtomicRecreationPtr = std::unique_ptr<AtomicRecreation>;

public:
    explicit ListHeuristicInsertion(SortingStrategyType strategy, EnumerationType enumeration);

    SortingStrategyType strategy; 
    EnumerationType enumeration;
    void reconstructSolution(Solution &solution, double blinkRate) const override;

private:
    /**
     * @param blinkRate probability to ignore the request insertion
     * @return the best insertion found
     */
    static std::unique_ptr<AtomicRecreation> selectRecreation(Solution &solution, Pair const &pair, double blinkRate,
                                                              EnumerationType enumeration);
};

/**
 * Used in enumerate.cpp functions to evaluate modification
 * Do not evaluate cost (see list_heuristic_cost_oriented that evaluate cost before feasability)
 * @tparam ModificationType the type of modification to be checked
 * @param solution the solution to check the modification validity
 * @param list the modification will be added to this list if valid
 * @return a function that takes a ModificationType and add it to list iff it is valid
 */
template<std::derived_from<AtomicRecreation> ModificationType>
std::function<void(ModificationType &&)> addToListIfValidTemplate(Solution const &solution,
                                                                  enumeration::ModificationContainer &list)
{
    return [&](ModificationType &&modification) {
        if (solution.checkModification(modification))
        {
            list.push_front(std::make_unique<ModificationType>(modification));
        }
    };
}