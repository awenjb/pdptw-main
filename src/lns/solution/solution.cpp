#include "solution.h"

#include "config.h"
#include "input/data.h"
#include "input/time_window.h"
#include "lns/constraints/capacity/capacity_constraint.h"
#include "lns/constraints/time_window/time_window_constraint.h"
#include "lns/solution/route.h"
#include "output/solution_checker.h"

#include <algorithm>
#include <bits/ranges_algo.h>
#include <bits/ranges_util.h>
#include <ranges>
#include <utility>
#include <vector>

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
    for (unsigned int i = 0; i < NUMBER_VEHICLE; ++i)
    {
        this->routes.emplace_back();
    }
}

void Solution::initConstraints()
{
    constraints.clear();
    constraints.push_back(std::make_unique<CapacityConstraint>(*this));
    constraints.push_back(std::make_unique<TimeWindowConstraint>(*this));
}

void Solution::computeAndStoreSolutionCost()
{
    rawCost = computeSolutionCost();

    // add penalty for solution in the pairBank ?
    totalCost = rawCost + computePenalisation();
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

double Solution::computePenalisation() const
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

Solution::Solution(PDPTWData const &data, Solution::PairBank pairbank, std::vector<Route> routes, double rawCost,
                   double totalCost)
    : data(data), pairBank(std::move(pairbank)), routes(std::move(routes)), rawCost(rawCost), totalCost(totalCost)
{}

Solution::Solution(PDPTWData const &data) : data(data)
{
    init();
}

Solution Solution::emptySolution(PDPTWData const &data)
{
    Solution sol = Solution(data);
    return sol;
}

Solution::~Solution() noexcept = default;

Solution::Solution(Solution const &rhs) : Solution(rhs.getData())
{
    *this = rhs;
}

Solution &Solution::operator=(Solution const &rhs)
{
    if (&rhs == this)
    {
        return *this;
    }

    data = rhs.data;
    rawCost = rhs.rawCost;
    totalCost = rhs.totalCost;
    pairBank = rhs.pairBank;

    routes.clear();
    routes = rhs.routes;

    constraints.clear();
    std::ranges::transform(rhs.constraints, std::back_inserter(constraints), [this](auto const &constraintPtr) {
        return constraintPtr->clone(*this);
    });

    return *this;
}

Solution::Solution(Solution &&sol) noexcept : data(sol.data)
{
    *this = std::move(sol);
}

Solution &Solution::operator=(Solution &&sol) noexcept
{
    if (&sol == this)
    {
        return *this;
    }

    data = sol.data;
    rawCost = sol.rawCost;
    totalCost = sol.totalCost;

    pairBank = std::move(sol.pairBank);
    routes = std::move(sol.routes);
    constraints = std::move(sol.constraints);

    for (auto &constraint: constraints)
    {
        constraint->setSolution(*this);
    }

    return *this;
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
    return routes.at(routeIndex);
}

Route const &Solution::getRoute(int routeIndex) const
{
    if (routeIndex < 0 || routeIndex >= routes.size())
    {
        spdlog::error("Invalid route index: {}", routeIndex);
        throw std::out_of_range("Invalid route index.");
    }
    return routes.at(routeIndex);
}

double Solution::getCost() const
{
    return rawCost + computePenalisation();
}

double Solution::getRawCost() const
{
    return rawCost;
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

int Solution::getRouteIDOf(int locationID) const
{
    int routeIndex = 0;
    for (Route const &route: getRoutes())
    {
        std::vector<int> const &routeLocationIDs = route.getRoute();
        if (std::ranges::find(routeLocationIDs, locationID) != routeLocationIDs.end())
        {
            return routeIndex;
        }
        ++routeIndex;
    }
    // no routes contain the location
    return -1;
}

unsigned int Solution::missingPairCount() const
{
    return pairBank.size();
}


bool Solution::checkModification(AtomicRecreation const &modification) const
{
    //std::cout << "--- Check Modification Validity : ";
    ModificationCheckVariant const &checkVariant = modification.asCheckVariant();
    // visitor pattern
    for (std::unique_ptr<Constraint> const &constraint: constraints)
    {
        if (!constraint->checkVariant(checkVariant))
        {
            //std::cout << "\n";
            return false;
        }
    }
    //std::cout << "\n";
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
    std::cout << "\nRawCost : " << rawCost << "\n"
              << "TotalCost : " << totalCost << "\n";

    std::cout << "Pair Bank : \n";

    for (int const id: getBank())
    {
        std::cout << id << ", ";
    }

    std::cout << "\nRoutes : \n";
    int i = 0;
    for (Route const &route: getRoutes())
    {
        std::cout << "#" << i << ": ";
        route.print();
        ++i;
    }

    std::cout << "Constraints : \n";
    for (std::unique_ptr<Constraint> const &constraint: constraints)
    {
        constraint->print();
    }
    std::cout << "\n";
}