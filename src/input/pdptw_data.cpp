#include "pdptw_data.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>


unsigned int PDPTWData::getSize()
{
    return size;
}

int PDPTWData::getCapacity()
{
    return capacity;
}

std::vector<Location> const &PDPTWData::getLocations() const 
{
    return locations;
}

Location const &PDPTWData::getDepot() const
{
    return depot;
}

Location const &PDPTWData::getLocation(int id) const
{
    return locations[id];
}

Matrix const &PDPTWData::getMatrix() const
{
    return distanceMatrix;
}

PDPTWData::PDPTWData(unsigned int size, int capacity, Location depot, std::vector<Location> location, Matrix distanceMatrix)
    : size(size), capacity(capacity), depot(depot), locations(std::move(location)), distanceMatrix(std::move(distanceMatrix)) {}


void PDPTWData::print() const 
{
    std::cout << "Instance size: " << size << "\n";
    std::cout << "Capacity: " << capacity << "\n";
    std::cout << "Depot:\n";
    depot.print();

    std::cout << "Locations:\n";
    for (const auto& loc : locations) {
        loc.print();
    }

    std::cout << "Distance Matrix:\n";
    for (const auto& row : distanceMatrix) {
        for (const auto& dist : row) {
            std::cout << dist << " ";
        }
        std::cout << "\n";
    }
}

void PDPTWData::checkData() const
{
    bool errorFlag = checkMatrix();

    errorFlag = checkLocation() || errorFlag;

    //errorFlag = checkTimeWindow() || errorFlag;

    if (errorFlag)
    {
        throw InputJsonException("failed to pass consistency checks");
    }
}


bool PDPTWData::checkMatrix() const
{
    // square matrix
    
    for (const auto& row : getMatrix()) 
    {
        if (row.size() != size) 
        {
            return true;
        }
    }

    /*
    for (int i = 0; i < size; i++) 
    {
        for (int j = 0; j < size; j++) 
        {
            for (int k = 0; k < size; k++) 
            {
                if (getMatrix()[i][j] > getMatrix()[i][k] + getMatrix()[k][j]) 
                {
                    return true;
                }
            }
        }
    }
    */

    return false;
}

bool PDPTWData::checkLocation() const
{

    // check if location id equals the position in the location vector
    for (size_t i = 0; i < size-1; ++i) {
        if (locations[i].getId() != static_cast<int>(i)+1) {
            return true;
        }
    }
    
    // check if pair of location are well made (type, id, demand, timeWindow)
    for(const Location& loc : getLocations()) 
    {
        if (loc.getLocType() == LocType::PICKUP)
        {   
            if ( (getLocations()[loc.getPair()-1].getLocType() != LocType::DELIVERY) 
            || (loc.getDemand() != - getLocations()[loc.getPair()-1].getDemand()) 
            || (loc.getId() != getLocations()[loc.getPair()-1].getPair()) )
            {
                return true;
            }
        }

        if (loc.getTimeWindow().getStart() > loc.getTimeWindow().getEnd())
        {
            return true;
        }
    }

    return false;
}


InputJsonException::InputJsonException(std::string_view reason)
    : reason(fmt::format("Input JSON file is incorrect : {}", reason))
{
    spdlog::default_logger()->flush();
}

char const *InputJsonException::what() const noexcept
{
    return reason.data();
}
