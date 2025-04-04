#pragma once

#include "lns/operators/abstract_operator.h"
#include "lns/operators/destruction/string_removal.h"

/*
 *  String removal applied to unfullfilled requests.
 *  Aims to reduce the number of routes by applying SISR operators on unfullfilled requests.
 *  If all requests are fullfilled, remove a route and try again.
 */
class BankFocusStringRemoval : public DestructionOperator
{
public:
    explicit BankFocusStringRemoval(unsigned int maxCardinalityOfString, unsigned int averageNumberRemovedElement)
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
