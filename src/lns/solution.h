#pragma once

#include <vector>
#include "route.h"

/**
 * Represent a solution of PDPTW
 */
class Solution
{
public:
    using RequestBank = std::vector<int>;

private:
    RequestBank bank;
    std::vector<Route> routes;
    int totalCost;

public:
    Solution(RequestBank bank, std::vector<Route> routes, int totalCost);
    const RequestBank & getBank() const;
    const std::vector<Route> & getRoute() const;
    int getCost();

};