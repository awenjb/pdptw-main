#include "solution.h"

#include "config.h"
#include "input/data.h"
#include "input/time_window.h"
#include "lns/constraints/capacity/capacity_constraint.h"
#include "lns/constraints/constraint.h"
#include "lns/constraints/time_window/time_window_constraint.h"
#include "lns/constraints/time_window_ltt/time_window_ltt_constraint.h"
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
        routes.emplace_back();
    }
}

void Solution::initConstraints()
{
    constraints.clear();
    constraints.push_back(std::make_unique<CapacityConstraint>(*this));
    constraints.push_back(std::make_unique<TimeWindowConstraint>(*this));

    if (ELEVATION)
    {
        lttConstraint = std::make_unique<TimeWindowLTTConstraint>(*this);
    }
}

void Solution::computeAndStoreSolutionCost()
{
    rawCost = computeSolutionCost();
    totalCost = rawCost + computePenalisation();
}

double Solution::computeSolutionCost() const
{
    double cost = 0.0;
    for (Route const &route: getRoutes())
    {
        cost += data::routeCost(data, route);
    }
    return cost;
}

double Solution::computePenalisation() const
{
    return getBank().size() * EXCLUSION_PENALTY + getNumberOfRoutes() * ROUTE_PENALTY;
}

int Solution::getNumberOfRoutes() const
{
    int count = 0;
    for (Route const &route: getRoutes())
    {
        if (!route.getRoute().empty())
        {
            count++;
        }
    }
    return count;
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
    Solution sol(data);
    return sol;
}

Solution::~Solution() noexcept = default;

Solution::Solution(Solution const &rhs) : Solution(rhs.getData())
{
    *this = rhs;
}

Solution &Solution::operator=(Solution const &rhs)
{
    if (this == &rhs) return *this;

    data = rhs.data;
    rawCost = rhs.rawCost;
    totalCost = rhs.totalCost;
    pairBank = rhs.pairBank;

    routes = rhs.routes;
    constraints.clear();
    std::ranges::transform(rhs.constraints, std::back_inserter(constraints), [this](auto const &constraintPtr) {
        return constraintPtr->clone(*this);
    });

    if (ELEVATION)
    {
        if (rhs.lttConstraint)
        {
            lttConstraint = rhs.lttConstraint->clone(*this);
        }
        else
        {
            lttConstraint.reset();
        }
    }

    return *this;
}

Solution::Solution(Solution &&sol) noexcept : data(sol.data)
{
    *this = std::move(sol);
}

Solution &Solution::operator=(Solution &&sol) noexcept
{
    if (this == &sol)
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

    if (ELEVATION)
    {
        lttConstraint = std::move(sol.lttConstraint);
        if (lttConstraint)
        {
            lttConstraint->setSolution(*this);
        }
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
    return routes.at(routeIndex);
}

Route const &Solution::getRoute(int routeIndex) const
{
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

std::vector<std::unique_ptr<Constraint>> const &Solution::getConstraints() const
{
    return constraints;
}

std::unique_ptr<Constraint> const &Solution::getLttConstraint() const
{
    return lttConstraint;
}

int Solution::requestsFulfilledCount() const
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
    ModificationCheckVariant const &checkVariant = modification.asCheckVariant();

    for (auto const &constraint: constraints)
    {
        if (!constraint->checkVariant(checkVariant))
        {
            return false;
        }
    }
    return true;
}

bool Solution::checkModificationLTT(AtomicRecreation const &modification) const
{
    ModificationCheckVariant const &checkVariant = modification.asCheckVariant();

    return lttConstraint->checkVariant(checkVariant);
}

void Solution::beforeApplyModification(AtomicModification &modification) const
{
    // Pre-modification check
    check();
}

void Solution::afterApplyModification(AtomicModification &modification)
{
    // Constraint status update
    for (auto &constraint: constraints)
    {
        constraint->applyVariant(modification.asApplyVariant());
    }

    if (ELEVATION)
    {
        lttConstraint->applyVariant(modification.asApplyVariant());
    }
}

void Solution::applyRecreateSolution(AtomicRecreation &modification)
{
    beforeApplyModification(modification);
    modification.modifySolution(*this);

    // Update the request bank
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

    // Update the request bank
    std::vector<int> const &deletedPair = modification.getDeletedPairs();
    pairBank.insert(pairBank.end(), deletedPair.begin(), deletedPair.end());

    afterApplyModification(modification);
}

void Solution::check() const
{
    checker::checkSolutionCoherence(*this, getData());
}

double Solution::getTotalDistance() const
{
    return 0;
}

double Solution::getTotalDuration() const
{
    return 0;
}

void Solution::print() const
{
    std::cout << "\nRawCost: " << rawCost << "\n"
              << "TotalCost: " << totalCost << "\n";

    std::cout << "Pair Bank:\n";
    for (int const id: getBank())
    {
        std::cout << id << ", ";
    }

    std::cout << "\nRoutes:\n";
    int i = 0;
    for (Route const &route: getRoutes())
    {
        std::cout << "#" << i << ": ";
        route.print();
        ++i;
    }

    std::cout << "\nTW :\n";
    auto const &tw = constraints.at(1);
    tw->print();


    // std::cout << "Constraints:\n";
    // for (auto const &constraint: constraints)
    // {
    //     constraint->print();
    // }

    std::cout << "\n";
}
