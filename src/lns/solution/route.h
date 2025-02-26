#pragma once

#include <vector>
#include "./../../input/time_window.h"

/**
 * Represent a route for the PDPTW
 * A route does not include the depot at the begining and the end !
 */
class Route
{

private:
    std::vector<int> route;
    int cost;
    


    /* Stocké dans les contraintes
    std::vector<TimeInteger> reach_time; // debut d'arrivee
    std::vector<double> FTS; // forward time slack
    std::vector<std::vector<double>> acc_cap; // inscreasing capacity allowed between two positions
    */
   
public:

    Route();
    Route(std::vector<int> route, int cost);
    int getCost() const;
    const std::vector<int> & getRoute() const;

    // get Location
    int getLocation(int index) const;    

    void print() const;

    /*
    * Add a location index in the route (does not update the route cost)
    */
    void insertAt(int locationIndex, int position);

    /*
    * Remove the element at "position" 
    */
    void deleteAt(int position);

    int getSize() const;
};