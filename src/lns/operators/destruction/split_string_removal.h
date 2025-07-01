#pragma once

#include "lns/operators/abstract_operator.h"

class SplitStringRemoval : public DestructionOperator
{
public:
    explicit SplitStringRemoval(unsigned int maxCardinalityOfString, unsigned int averageNumberRemovedElement)
        : maxCardinalityOfString(maxCardinalityOfString), averageNumberRemovedElement(averageNumberRemovedElement)
    {}

    // Each string has a max number of requests: L_max in the paper.
    unsigned int maxCardinalityOfString = 10;

    // This operator will remove on average this number of requests.
    // c̄ in the paper.
    unsigned int averageNumberRemovedElement = 10;

    /**
     * Applies the string removal operator to the given solution,
     * removing pickup-delivery pairs around a preserved subsequence.
     */
    void destroySolution(Solution &solution) const override;

private:
    int numberOfPairsToDestroy;
};

namespace sisr
{
    /**
     * Computes the average number of locations per route.
     */
    double computeAverageCardinalitySplit(std::vector<Route> const &routes);

    /**
     * Remove a string of locations but preserve a subsequence inside it.
     */
    void removeAroundSubsequenceSplit(Solution &solution, int routeIndex, unsigned int stringLength, unsigned int preserveLength, int startLocation);

    /**
     * Main ruin function as described in the SISR paper, with preserved subsequences.
     */
    void SISRsRuinSplit(Solution &solution, unsigned int maxStringSize, unsigned int averageNumberRemovedElement);
} // namespace sisr
