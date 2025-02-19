#pragma once

#include <vector>

/**
 * Represent a route for the PDPTW
 */
class Route
{

private:
    std::vector<int> route;
    int cost;

public:

    Route(std::vector<int> route, int cost);
    int getCost() const;
    const std::vector<int>& getRoute() const;
    
   
};