#pragma once

#include "input/data.h"
#include "input/pdptw_data.h"
#include "lns/operators/abstract_operator.h"
#include "lns/operators/generators/modification_generator.h"
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
template<std::derived_from<sorting_strategy::SortingStrategy> Strategy,
         std::derived_from<generator::ModificationGenerator> Generator>
class ListHeuristicInsertion : public ReconstructionOperator
{
private:
    using AtomicRecreationPtr = std::unique_ptr<AtomicRecreation>;

public:
    explicit ListHeuristicInsertion();

    void reconstructSolution(Solution &solution, double blinkRate) const override;

private:
    /**
     * @param blinkRate probability to ignore the request insertion
     * @return the best insertion found
     */
    static std::unique_ptr<AtomicRecreation> choosingStrategy(Solution &solution, Pair const &pair,
                                                              double blinkRate);
};
