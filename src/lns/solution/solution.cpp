#include "solution.h"

#include <utility>

#include "input/data.h"
#include "input/time_window.h"
#include "lns/constraints/capacity/capacity_constraint.h"
#include "lns/constraints/time_window/time_window_constraint.h"

#include "config.h"
#include "lns/solution/route.h"

void Solution::initPairBank()
{   
    pairBank.clear();
    for (const Pair & pair : getData().getPairs())
    {
        pairBank.push_back(pair.getID());
    }
}

void Solution::initRoutes()
{
    routes.clear();
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
    for (const Route & route : getRoutes())
    {
        cost += data::routeCost(data, route);
    }
    std::cout << "le cout " << cost << " \n";
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


Solution::Solution(const PDPTWData &data, Solution::PairBank pairbank, std::vector<Route> routes, double routeCost, double totalCost) :
    data(data), 
    pairBank(std::move(pairbank)), 
    routes(std::move(routes)), 
    routeCost(routeCost),
    totalCost(totalCost) {}

Solution::Solution(const PDPTWData &data) : data(data)
{
    init();
}

Solution Solution::emptySolution(const PDPTWData &data)
{
    Solution s = Solution(data);
    return s;
}

const Solution::PairBank & Solution::getBank() const
{
    return pairBank;
}

const std::vector<Route> & Solution::getRoutes() const
{
    return routes;
}

std::vector<Route> & Solution::getRoutes()
{
    return routes;
}

Route & Solution::getRoute(int routeIndex) 
{
    if (routeIndex < 0 || routeIndex >= routes.size()) 
    {
        spdlog::error("Invalid route index: {}", routeIndex);
        throw std::out_of_range("Invalid route index.");
    }
    return routes[routeIndex];
}

const Route & Solution::getRoute(int routeIndex) const
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

const PDPTWData & Solution::getData() const
{
    return data;
}

void Solution::print() const
{
    std::cout << "Cost :" << totalCost << "\n" << "Routes : \n";

    for (const Route& id : getRoutes())
    {
        id.print();
    }

    std::cout << "Banques : \n";

    for (const int id : getBank())
    {
        std::cout << id << ", ";
    }
    std::cout << "\n";
}