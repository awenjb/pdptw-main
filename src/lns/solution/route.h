#pragma once

#include <vector>

#include "input/pdptw_data.h"
#include "input/time_window.h"

/**
 * Represent a route for the PDPTW
 * A route does not include the depot at the begining and the end !
 */
class Route
{

private:
    std::vector<int> route;
    int cost;
    
   
public:

    Route();
    Route(std::vector<int> route, int cost);
    int getCost() const;
    const std::vector<int> & getRoute() const;

    // get Location
    int getLocation(int index) const;    

    /**
     *  Given a locationID, return the Index in the route.
     *  return -1 if no such location.
     */
    int getIndex(int locationID) const;
    
    /**
     *  Given the position of a location in a route, return the paired location position.
     *  Example, a pair of location ID 3 and 4 are in a route at position 7 and 18, 
     *  getPairLocationPosition(7) must return 18 and vice versa.
     */
    int getPairLocationPosition(int position, const PDPTWData &data) const;


    void print() const;

    /**
     *  Add a location index in the route (does not update the route cost)
     */
    void insertAt(int locationIndex, int position);

    /*
    * Remove the element at "position" 
    */
    void deleteAt(int position);

    int getSize() const;
};