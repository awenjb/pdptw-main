#pragma once

#include "lns/operators/abstract_operator.h"

class StringRemoval : public DestructionOperator
{
public:
    explicit StringRemoval(unsigned int maxCardinalityOfString, unsigned int averageNumberRemovedElement)
        : maxCardinalityOfString(maxCardinalityOfString), averageNumberRemovedElement(averageNumberRemovedElement)
    {}

    // Each string has a max number of requests: MAX_CARDINALITY_OF_STRING
    // L_max in the paper.
    unsigned int maxCardinalityOfString = 10;
    // This operator will remove in average AVERAGE_CUSTOMER_REMOVAL of requests
    // c with a bar on top in the paper
    unsigned int averageNumberRemovedElement = 10;

    /**
     * This operator removes numberOfPairsToDestroy pairs randomly in the solution.
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
    double computeAverageCardinality(std::vector<Route> const &routes);

    /**
     * Remove a string of locations.
     */
    void removeString(Solution &solution, int routeIndex, unsigned int stringLength, int startLocation);

    /**
     * Main ruin function as described in the SISR paper, with preserved subsequences.
     */
    void SISRsRuin(Solution &solution, unsigned int maxStringSize, unsigned int averageNumberRemovedElement);
};// namespace