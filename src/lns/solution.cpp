#include "solution.h"

Solution::Solution(RequestBank bank, std::vector<Route> routes, int totalCost)
    : bank(bank), routes(routes), totalCost(totalCost) {}

const std::vector<int> & Solution::getBank() const
{
    return bank;
}

const std::vector<Route> & Solution::getRoute() const
{
    return routes;
}

int Solution::getCost()
{
    return totalCost;
}