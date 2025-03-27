#pragma once

#include <vector>
#include "input/pdptw_data.h"
#include "input/time_window.h"

/**
 *  Represents a route for the PDPTW.
 *  A route does not include the depot at the beginning and the end.
 */
class Route
{
private:
    std::vector<int> route;  // Stores the route as a sequence of location IDs.
    int cost;  // The cost of the route.

public:
    // Constructors
    Route();
    Route(std::vector<int> route, int cost);

    // Getters
    int getCost() const;
    const std::vector<int>& getRoute() const;

    // Get the location at a given index in the route
    int getLocation(int index) const;

    /**
     * Given a locationID, returns the index in the route.
     * @return -1 if the locationID is not found in the route.
     */
    int getIndex(int locationID) const;

    /**
     * Given the position of a location in the route, returns the paired location position.
     * Example: if location ID 3 and 4 are in the route at positions 7 and 18,
     * getPairLocationPosition(7) should return 18 and vice versa.
     */
    int getPairLocationPosition(int position, const PDPTWData& data) const;

    // Utility function to print the route details
    void print() const;

    /**
     * Adds a location index in the route at a specified position.
     * Does not update the route cost.
     */
    void insertAt(int locationIndex, int position);

    /**
     * Removes the element at the specified position in the route.
     */
    void deleteAt(int position);

    // Returns the number of locations in the route
    int getSize() const;
};