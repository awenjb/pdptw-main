#pragma once

#include "input/pair.h"
#include "input/pdptw_data.h"
#include "lns/constraints/constraint.h"
#include "route.h"

#include <iostream>
#include <memory>
#include <spdlog/spdlog.h>
#include <vector>

class AtomicModification;
class AtomicRecreation;
class AtomicDestruction;
class TimeWindowConstraint;
class CapacityConstraint;

/*
 * Represents a solution of the PDPTW problem.
 */
class Solution
{
public:
    using PairBank = std::vector<int>;

private:
    std::reference_wrapper<const PDPTWData> data;// Reference to PDPTW data
    PairBank pairBank;                           // Unassigned pairs (Pickup & Delivery)
    std::vector<Route> routes;                   // Routes in the solution
    double rawCost;
    double totalCost;
    std::vector<std::unique_ptr<Constraint>> constraints;
    std::unique_ptr<Constraint> lttConstraint;  // Constraint used when elevation is taken into account

public:
    /*
     * Creates an empty solution with all pairs in the pairBank.
     */
    static Solution emptySolution(PDPTWData const &data);

    /*
     * In-depth copy of the solution.
     */
    Solution(Solution const &);
    /*
     * Copy assignment.
     */
    Solution &operator=(Solution const &);
    /*
     * Move constructor.
     */
    Solution(Solution &&) noexcept;
    /*
     * Move assignment.
     */
    Solution &operator=(Solution &&) noexcept;
    /*
     * Destructor.
     */
    ~Solution() noexcept;

    /*
     * Constructs a solution with given data.
     */
    explicit Solution(PDPTWData const &data);
    /*
     * Constructs a solution with specified parameters.
     */
    Solution(PDPTWData const &data, PairBank pairbank, std::vector<Route> routes, double routeCost, double totalCost);

    /*
     * Returns the pair bank.
     */
    PairBank const &getBank() const;
    /*
     * Returns the pair bank.
     */
    PairBank const &getPairBank() const;
    /*
     * Returns a mutable reference to the pair bank.
     */
    PairBank &getPairBank();
    /*
     * Returns the list of routes.
     */
    std::vector<Route> const &getRoutes() const;
    /*
     * Returns a mutable reference to a specific route.
     */
    Route &getRoute(int routeIndex);
    /*
     * Returns a constant reference to a specific route.
     */
    Route const &getRoute(int routeIndex) const;
    /*
     * Returns the PDPTW data reference.
     */
    PDPTWData const &getData() const;
    /*
     * Returns the raw cost of the solution.
     */
    double getRawCost() const;
    /*
     * Returns the total cost of the solution.
     */
    double getCost() const;
    /*
     * Returns the number of routes.
     */
    int getNumberOfRoutes() const;
    /*
     * Returns the count of missing pairs.
     */
    unsigned int missingPairCount() const;
    /*
     * Returns the constraints applied to the solution.
     */
    std::vector<std::unique_ptr<Constraint>> const &getConstraints() const;
    /*
     * Returns the cumulative distance of all routes.
     */
    double getTotalDistance() const;
    /*
     * Returns the cumulative duration of all routes.
     */
    double getTotalDuration() const;

    /*
     * Returns the route index associated with a given location ID, or -1 if not in a route.
     */
    int getRouteIDOf(int locationID) const;
    /*
     * Returns the number of fulfilled requests.
     */
    int requestsFulfilledCount() const;
    /*
     * Checks if the modification is valid according to all constraints.
     */
    bool checkModification(AtomicRecreation const &modification) const;
    /*
     * Checks if the modification is valid to load-dependent time windows constraints.
     */
    bool checkModificationLTT(AtomicRecreation const &modification) const;
    /*
     * Verifies the correctness of the solution, mainly for debugging.
     */
    void check() const;

    /*
     * Pre-modification check. The modification must be valid.
     */
    void beforeApplyModification(AtomicModification &modification) const;
    /*
     * Updates constraints after modification is applied.
     */
    void afterApplyModification(AtomicModification &modification);

    /*
     * Applies recreation modification to the solution without validity checks.
     */
    void applyRecreateSolution(AtomicRecreation &modification);
    /*
     * Applies destruction modification to the solution without validity checks.
     */
    void applyDestructSolution(AtomicDestruction &modification);

    /*
     * Computes and returns the penalty value.
     */
    double computePenalisation() const;
    /*
     * Returns a mutable reference to the routes.
     */
    std::vector<Route> &getRoutes();
    /*
     * Prints the solution details.
     */
    void print() const;
    /*
     * Computes and stores the total solution cost.
     */
    void computeAndStoreSolutionCost();

    std::unique_ptr<Constraint> const &getLttConstraint() const;

private:
    /*
     * Initializes the object, called by the constructor.
     */
    void init();
    /*
     * Initializes or resets the problem constraints.
     */
    void initConstraints();
    /*
     * Initializes the routes.
     */
    void initRoutes();
    /*
     * Initializes the pair bank.
     */
    void initPairBank();
    /*
     * Computes the total cost of the solution.
     */
    double computeSolutionCost() const;
};
