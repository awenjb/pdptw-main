#pragma once

#include <vector>
#include "route.h"
#include "./../../input/pdptw_data.h"

/**
 * Represent a solution of PDPTW
 */
class Solution
{
public:
    using RequestBank = std::vector<int>;

private:
    PDPTWData const & data;
    RequestBank bank;
    std::vector<Route> routes;
    int totalCost;

    static PDPTWData dummy;

public:
    Solution(const PDPTWData &data);
    Solution(const PDPTWData &data, RequestBank bank, std::vector<Route> routes, int totalCost);

    RequestBank const & getBank() const;

    std::vector<Route> const & getRoutes() const;
    Route const & getRoute(int routeIndex) const; 

    const PDPTWData & getData() const;

    // For route modification
    std::vector<Route> & getRoutes();
    Route & getRoute(int routeIndex);
    

    int getCost();

    void print() const;
};