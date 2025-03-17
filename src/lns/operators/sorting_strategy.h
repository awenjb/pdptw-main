#pragma once

#include "input/time_window.h"
#include "lns/solution/solution.h"

/**
 * A type of sorting strategy for the bank of pairs.
 */
enum class SortingStrategyType
{
    SHUFFLE,
    DEMAND,
    CLOSE,
    FAR,
    TWWIDTH,
    TWSTART,
    TWEND,
};

namespace sorting_strategy
{
    /**
     * Interface for sorting the requests in the request bank. Does modify directly the solution request bank.
     */
    class SortingStrategy
    {
    public:
        explicit SortingStrategy(Solution &solution) : solution(solution) {}

        Solution &getSolution() const { return solution; }

        virtual std::vector<int> const &sortPairs() const = 0;
        virtual ~SortingStrategy() = default;

    private:
        // non const to sort in place.
        Solution &solution;
    };

    /**
     * Shuffle the requests.
     */
    class Shuffle : public SortingStrategy
    {
    public:
        using SortingStrategy::SortingStrategy;
        std::vector<int> const &sortPairs() const override;
    };

    /**
     *  Sort the bank by decreasing order of demand.
     */
    class Demand : public SortingStrategy
    {
    public:
        using SortingStrategy::SortingStrategy;
        std::vector<int> const &sortPairs() const override;
    };

    /**
     *  Sort the bank by increasing distance from the depot.
     */
    class Close : public SortingStrategy
    {
    public:
        using SortingStrategy::SortingStrategy;
        std::vector<int> const &sortPairs() const override;
    };

    /**
     *  Sort the bank by decreasing distance from the depot.
     */
    class Far : public SortingStrategy
    {
    public:
        using SortingStrategy::SortingStrategy;
        std::vector<int> const &sortPairs() const override;
    };

    /**
     *  Sort the bank by increasing time window width.
     */
    class TimeWindowWidth : public SortingStrategy
    {
    public:
        using SortingStrategy::SortingStrategy;
        std::vector<int> const &sortPairs() const override;
    };

    /**
     *  Sort the bank by inscreasing time window start (compare the pickup time window).
     */
    class TimeWindowStart : public SortingStrategy
    {
    public:
        using SortingStrategy::SortingStrategy;
        std::vector<int> const &sortPairs() const override;
    };

    /**
     *  Sort the bank by decreasing time window end (compare the delivery time window).
     */
    class TimeWindowEnd : public SortingStrategy
    {
    public:
        using SortingStrategy::SortingStrategy;
        std::vector<int> const &sortPairs() const override;
    };

}// namespace sorting_strategy