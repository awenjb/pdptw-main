#include "solution.h"

#include "config.h"
#include "input/data.h"
#include "input/time_window.h"
#include "lns/constraints/capacity/capacity_constraint.h"
#include "lns/constraints/time_window/time_window_constraint.h"
#include "lns/solution/route.h"
#include "output/solution_checker.h"

#include <bits/ranges_util.h>
#include <utility>

void Solution::initPairBank()
{
    pairBank.clear();
    for (Pair const &pair: getData().getPairs())
    {
        pairBank.push_back(pair.getID());
    }
}

void Solution::initRoutes()
{
    routes.clear();
    routes.emplace_back();
}

void Solution::initConstraints()
{
    constraints.clear();
    constraints.push_back(std::make_unique<CapacityConstraint>(*this));
    constraints.push_back(std::make_unique<TimeWindowConstraint>(*this));
}

void Solution::computeAndStoreSolutionCost()
{
    routeCost = computeSolutionCost();

    // add penalty for solution in the pairBank ?
    totalCost = routeCost + computePenalization();
}

double Solution::computeSolutionCost() const
{
    double cost = 0;
    for (Route const &route: getRoutes())
    {
        cost += data::routeCost(data, route);
    }
    return cost;
}

double Solution::computePenalization() const
{
    return getBank().size() * EXCLUSION_PENALTY;
}

void Solution::init()
{
    initPairBank();
    initRoutes();
    initConstraints();
    computeAndStoreSolutionCost();
}

Solution::Solution(PDPTWData const &data, Solution::PairBank pairbank, std::vector<Route> routes, double routeCost,
                   double totalCost)
    : data(data), pairBank(std::move(pairbank)), routes(std::move(routes)), routeCost(routeCost), totalCost(totalCost)
{}

Solution::Solution(PDPTWData const &data) : data(data)
{
    init();
}

Solution Solution::emptySolution(PDPTWData const &data)
{
    Solution s = Solution(data);
    return s;
}

Solution::PairBank const &Solution::getBank() const
{
    return pairBank;
}

Solution::PairBank const &Solution::getPairBank() const
{
    return pairBank;
}

Solution::PairBank &Solution::getPairBank()
{
    return pairBank;
}

std::vector<Route> const &Solution::getRoutes() const
{
    return routes;
}

std::vector<Route> &Solution::getRoutes()
{
    return routes;
}

Route &Solution::getRoute(int routeIndex)
{
    if (routeIndex < 0 || routeIndex >= routes.size())
    {
        spdlog::error("Invalid route index: {}", routeIndex);
        throw std::out_of_range("Invalid route index.");
    }
    return routes[routeIndex];
}

Route const &Solution::getRoute(int routeIndex) const
{
    if (routeIndex < 0 || routeIndex >= routes.size())
    {
        spdlog::error("Invalid route index: {}", routeIndex);
        throw std::out_of_range("Invalid route index.");
    }
    return routes[routeIndex];
}

double Solution::getCost() const
{
    return totalCost;
}

PDPTWData const &Solution::getData() const
{
    return data.get();
}

int Solution::requestsFulFilledCount() const
{
    int count = 0;
    for (Route const &route: getRoutes())
    {
        count += route.getRoute().size() / 2;
    }
    return count;
}

bool Solution::checkModification(AtomicRecreation const &modification) const 
{
    std::cout << "--- Check Modification Validity : ";
    ModificationCheckVariant const &checkVariant = modification.asCheckVariant();
    // visitor pattern
    for (std::unique_ptr<Constraint> const &constraint: constraints)
    {
        if (!constraint->checkVariant(checkVariant))
        {
            std::cout << "\n";
            return false;
        }
    }
    std::cout << "\n";
    return true; 
}


void Solution::beforeApplyModification(AtomicModification &modification)
{
    // pre modification check
    check();
}

void Solution::afterApplyModification(AtomicModification &modification)
{
    // constraint status update
    for (std::unique_ptr<Constraint> &constraint: constraints)
    {
        constraint->applyVariant(modification.asApplyVariant());
    }
}

void Solution::applyRecreateSolution(AtomicRecreation &modification)
{
    beforeApplyModification(modification);

    modification.modifySolution(*this);
    // we update the request bank
    if (int pairID = modification.getAddedPairs())
    {
        pairBank.erase(std::ranges::find(pairBank, pairID));
    }

    afterApplyModification(modification);
}

void Solution::applyDestructSolution(AtomicDestruction &modification)
{
    beforeApplyModification(modification);

    modification.modifySolution(*this);
    // updating request bank
    std::vector<int> const &deletedPair = modification.getDeletedPairs();
    
    //pairBank.reserve(pairBank.size() + deletedPair.size()); 
    pairBank.insert(pairBank.end(), deletedPair.begin(), deletedPair.end());

    afterApplyModification(modification);
}

void Solution::check() const
{
   checker::checkSolutionCoherence(*this, getData());
}

void Solution::print() const
{
    std::cout << "Cost : " << totalCost << "\n"
              << "Routes : \n";

    for (Route const &id: getRoutes())
    {
        id.print();
    }

    std::cout << "Pair Bank : \n";

    for (int const id: getBank())
    {
        std::cout << id << ", ";
    }
    std::cout << "\n";
}