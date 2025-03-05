#pragma once

#include "input/pair.h"
#include "lns/constraints/constraint.h"
#include "route.h"

#include <iostream>
#include <spdlog/spdlog.h>
#include <vector>

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
    // ref_wrapper
    PDPTWData const &data;
    /*
     *  Store IDs of a pairs (Pickup & Delivery) that are not assigned yet to a route.
     */
    PairBank pairBank;
    /*
     *  Vector of routes representing the solution
     */
    std::vector<Route> routes;
    double routeCost;
    double totalCost;
    std::vector<std::unique_ptr<Constraint>> constraints;

public:
    /**
     *  Expected way to construct a solution.
     */
    static Solution emptySolution(PDPTWData const &data);

    explicit Solution(PDPTWData const &data);

    /**
     *  For testing/debugging.
     *  Use emptySolution(const PDPTWData &data) to create an initial empty solution.
     */
    Solution(PDPTWData const &data, Solution::PairBank pairbank, std::vector<Route> routes, double routeCost,
             double totalCost);


    PairBank const &getBank() const;
    PairBank const &getPairBank() const;
    PairBank &getPairBank();
    std::vector<Route> const &getRoutes() const;
    Route const &getRoute(int routeIndex) const;
    PDPTWData const &getData() const;
    double getCost() const;

    /**
     *  Return the number of fullfilled requests.
     *  The solution must be consistent !
     *  (compute this number by looking at the size of each routes)
     */
    int requestsFulFilledCount() const;

    /**
     * Check that the modification is valid regarding all the constraints
     * @param modification
     * @return true if the modification is valid
     */
    bool checkModification(AtomicRecreation const &modification) const;

    /**
     *  Pre modification check.
     *  @param modification Must be a valid modification.
     */
    void beforeApplyModification(AtomicModification &modification);

    /**
     *  Update constraints, it is expected that the solution has already been modified.
     *  @param modification Must be a valid modification.
     */
    void afterApplyModification(AtomicModification &modification);

    /**
     *  Apply the given the modification on the solution. Does not check validity.
     *  @param modification Must be a valid recreation.
     */
    void applyRecreateSolution(AtomicRecreation &modification);

    /**
     *  Apply the given the modification on the solution. Does not check validity.
     *  @param modification Must be a destruction.
     */
    void applyDestructSolution(AtomicDestruction &modification);


    // For route modification
    std::vector<Route> &getRoutes();
    Route &getRoute(int routeIndex);

    void print() const;


private:
    /**
     *  Does the initialisation of the object, called by the constructor
     */
    void init();

    /**
     *  Init/reset the constraints of the problem
     */
    void initConstraints();
    void initRoutes();
    void initPairBank();

    void computeAndStoreSolutionCost();

    /**
     *  Compute the cost of the solution (routes cost)
     */
    double computeSolutionCost() const;

    double computePenalization() const;
};