#include "pdptw_data.h"

#include <iostream>
#include <fstream>
#include <spdlog/spdlog.h>


int PDPTWData::getSize() const
{
    return size;
}

int PDPTWData::getCapacity() const
{
    return capacity;
}

std::vector<Location> const &PDPTWData::getLocations() const 
{
    return locations;
}

std::vector<Pair> const &PDPTWData::getPairs() const 
{
    return pairs;
}

Location const &PDPTWData::getDepot() const
{
    return depot;
}

Location const &PDPTWData::getLocation(int id) const
{
    if (id==0)
    {
        return getDepot();
    }
    // location index from 0 to n-1
    return locations.at(id -1);
}

Matrix const &PDPTWData::getMatrix() const
{
    return distanceMatrix;
}

PDPTWData::PDPTWData(int size, int capacity, Location depot, std::vector<Location> locations, Matrix distanceMatrix)
    : size(size), capacity(capacity), depot(depot), locations(std::move(locations)), distanceMatrix(std::move(distanceMatrix)) 
{
    // Associate pair of locations
    pairs.clear();
    for (const Location & loc : this->locations)
    {
        if( loc.getLocType() == LocType::PICKUP )
        {
            // vector indexed from 0 / Location indexed from 1
            pairs.emplace_back(loc, this->locations.at(loc.getPair()-1), loc.getId());
        }
    }
}

const Pair &PDPTWData::getPair(int id) const
{
    for (const Pair &pair : pairs)
    {
        if (id == pair.getID())
        {
            return pair;
        }
    }
    spdlog::error("Pair not found for ID {}", id);
    throw std::runtime_error("Pair not found");
}

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

    std::cout << "Pair IDs:\n";
    for (const auto& pair : pairs)
    {;
        std::cout << pair.getID() << " "; 
    }
    std::cout << " \n";
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
                if (getMatrix().at(i).at(j) > getMatrix().at(i).at(k) + getMatrix().at(k).at(j)) 
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
        if (locations.at(i).getId() != static_cast<int>(i)+1) {
            return true;
        }
    }
    
    // check if pair of location are well made (type, id, demand, timeWindow)
    for(const Location& loc : getLocations()) 
    {
        if (loc.getLocType() == LocType::PICKUP)
        {   
            if ( (getLocations().at(loc.getPair()-1).getLocType() != LocType::DELIVERY) 
            || (loc.getDemand() != - getLocations().at(loc.getPair()-1).getDemand()) 
            || (loc.getId() != getLocations().at(loc.getPair()-1).getPair()) )
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
