#pragma once

#include <vector>
#include <iostream>
#include <spdlog/spdlog.h>


#include "lns/constraints/constraint.h"
#include "route.h"
#include "input/pair.h"

class AtomicModification;
class AtomicRecreation;
class AtomicDestruction;
class TimeWindowConstraint;
class CapacityConstraint;

/**
 * Represent a solution of PDPTW
 */
class Solution
{
public:
    using PairBank = std::vector<int>;

private:
    PDPTWData const & data;
    /*
    *   Store IDs of a pairs (Pickup & Delivery) that are not assigned yet to a route.
    */
    PairBank pairBank;
    /*
    *   Vector of routes representing the solution
    */
    std::vector<Route> routes;
    double routeCost;
    double totalCost;
    std::vector<std::unique_ptr<Constraint>> constraints;

public:

    /**
    *   Expected way to construct a solution.
    */
    static Solution emptySolution(const PDPTWData &data);

    explicit Solution(const PDPTWData &data);

    /**
    *   For testing/debugging.
    *   Use emptySolution(const PDPTWData &data) to create an initial empty solution.
    */
    Solution(const PDPTWData &data, Solution::PairBank pairbank, std::vector<Route> routes, double routeCost, double totalCost);


    PairBank const & getBank() const;
    std::vector<Route> const & getRoutes() const;
    Route const & getRoute(int routeIndex) const; 
    const PDPTWData & getData() const;
    double getCost() const;

    // For route modification
    std::vector<Route> & getRoutes();
    Route & getRoute(int routeIndex);
    
    void print() const;


private :

    /**
    *   Does the initialisation of the object, called by the constructor
    */
    void init();

    /**
    *   Init/reset the constraints of the problem
    */
    void initConstraints();
    void initRoutes();
    void initPairBank();
 
    void computeAndStoreSolutionCost();
    
    /**
    *   Compute the cost of the solution (routes cost)
    */
    double computeSolutionCost() const;
    
    double computePenalization() const;
};