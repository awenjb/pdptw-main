#pragma once

#include "lns/solution/solution.h"

/**
 * A type of sorting strategy for the bank of pairs
 */
enum class SortingStrategyType {
    SHUFFLE
};

namespace sorting_strategy
{
    /**
     * Interface for sorting the requests in the request bank. Does modify directly the solution request bank
     */
    class SortingStrategy
    {
    public:
        explicit SortingStrategy(Solution &solution) : solution(solution) {}

        Solution &getSolution() const { return solution; }

        virtual std::vector<int> const &sortPairs() const = 0;
        virtual ~SortingStrategy() = default;

    private:
        // non const to sort in place
        Solution &solution;
    };

    /**
     * Shuffle the requests
     */
    class Shuffle : public SortingStrategy
    {
        using SortingStrategy::SortingStrategy;

    public:
        std::vector<int> const &sortPairs() const override;
    };


}// namespace sorting_strategy