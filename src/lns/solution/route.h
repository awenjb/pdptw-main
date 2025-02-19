#pragma once

#include <vector>
#include "./../../input/time_window.h"

/**
 * Represent a route for the PDPTW
 */
class Route
{

private:
    int cost;
    std::vector<int> route;


    /* Stocké dans les contraintes
    std::vector<TimeInteger> reach_time; // debut d'arrivee
    std::vector<double> FTS; // forward time slack
    std::vector<std::vector<double>> acc_cap; // inscreasing capacity allowed between two positions
    */
   
public:

    Route(std::vector<int> route, int cost);
    int getCost() const;
    const std::vector<int>& getRoute() const;
    void print() const;
   
};