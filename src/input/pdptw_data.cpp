#include "pdptw_data.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>


unsigned int PDPTWData::getSize()
{
    return size;
}

int PDPTWData::getCapacity()
{
    return capacity;
}

std::vector<Location> const &PDPTWData::getLocations() const {
    return locations;
}

Location const PDPTWData::getDepot() const{
    return depot;
}


PDPTWData::PDPTWData(unsigned int size, int capacity, Location depot, std::vector<Location> location, Matrix distance_matrix)
    : size(size), capacity(capacity), depot(depot), locations(std::move(location)), distance_matrix(std::move(distance_matrix)) {}


void PDPTWData::print() const {
    std::cout << "Instance size: " << size << "\n";
    std::cout << "Capacity: " << capacity << "\n";
    std::cout << "Depot:\n";
    depot.print();

    std::cout << "Locations:\n";
    for (const auto& loc : locations) {
        loc.print();
    }

    std::cout << "Distance Matrix:\n";
    for (const auto& row : distance_matrix) {
        for (const auto& dist : row) {
            std::cout << dist << " ";
        }
        std::cout << "\n";
    }
}