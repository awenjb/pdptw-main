#include "pdptw_data.h"

#include "data.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
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

std::string PDPTWData::getDataName() const
{
    return dataName;
}

Location const &PDPTWData::getLocation(int id) const
{
    if (id == 0)
    {
        return getDepot();
    }
    // location index from 0 to n-1
    return locations.at(id - 1);
}

Matrix const &PDPTWData::getMatrix() const
{
    return costMatrix;
}

PDPTWData::PDPTWData(std::string dataName, int size, int capacity, Location depot, std::vector<Location> locations,
                     Matrix costMatrix)
    : dataName(dataName), size(size), capacity(capacity), depot(depot), locations(std::move(locations)),
      costMatrix(std::move(costMatrix))
{
    // Associate pair of locations
    pairs.clear();
    for (Location const &loc: this->locations)
    {
        if (loc.getLocType() == LocType::PICKUP)
        {
            // vector indexed from 0 / Location indexed from 1
            pairs.emplace_back(loc, this->locations.at(loc.getPair() - 1), loc.getId());
        }
    }
    // Compute closest location matrix
    initClosestLocations();
}

Pair const &PDPTWData::getPair(int id) const
{
    for (Pair const &pair: pairs)
    {
        if (id == pair.getID())
        {
            return pair;
        }
    }
    spdlog::error("Pair not found for ID {}", id);
    throw std::runtime_error("Pair not found");
}

int PDPTWData::getPairCount() const
{
    return getPairs().size();
}

int PDPTWData::getLocationCount() const
{
    return getLocations().size();
}

void PDPTWData::print() const
{
    std::cout << "Instance name : " << dataName << "\n";
    std::cout << "Instance size: " << size << "\n";
    std::cout << "Capacity: " << capacity << "\n";
    std::cout << "Depot:\n";
    depot.print();

    std::cout << "Locations:\n";
    for (auto const &loc: locations)
    {
        loc.print();
    }

    std::cout << "Distance Matrix:\n";
    for (auto const &row: costMatrix)
    {
        for (auto const &dist: row)
        {
            std::cout << dist << " ";
        }
        std::cout << "\n";
    }

    std::cout << "Closest Matrix (no depot):\n";
    for (auto const &row: closestLocations)
    {
        for (auto const &ID: row)
        {
            std::cout << ID << " ";
        }
        std::cout << "\n";
    }

    std::cout << "Pair IDs:\n";
    for (auto const &pair: pairs)
    {
        ;
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

    for (auto const &row: getMatrix())
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
    for (size_t i = 0; i < size - 1; ++i)
    {
        if (locations.at(i).getId() != static_cast<int>(i) + 1)
        {
            return true;
        }
    }

    // check if pair of location are well made (type, id, demand, timeWindow)
    for (Location const &loc: getLocations())
    {
        if (loc.getLocType() == LocType::PICKUP)
        {
            if ((getLocations().at(loc.getPair() - 1).getLocType() != LocType::DELIVERY) ||
                (loc.getDemand() != -getLocations().at(loc.getPair() - 1).getDemand()) ||
                (loc.getId() != getLocations().at(loc.getPair() - 1).getPair()))
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

void PDPTWData::initClosestLocations()
{
    closestLocations.reserve(getLocationCount());
    for (Location const &location: locations)
    {
        // sorting from closest to furthest using indexes
        std::vector<int> closestLocationsIndexes(getLocationCount());
        std::iota(closestLocationsIndexes.begin(), closestLocationsIndexes.end(), 0);

        // Do the sorting
        std::ranges::sort(closestLocationsIndexes, {}, [this, &location](int index) {
            return data::TravelTime(*this, location.getId(), locations.at(index).getId());
        });

        // we store the indexes in the final container
        closestLocations.emplace_back();
        closestLocations.back().reserve(getLocationCount());
        std::ranges::transform(closestLocationsIndexes, std::back_inserter(closestLocations.back()), [this](int index) {
            return std::cref(locations.at(index)).get().getId();
        });
    }
}

std::vector<int> const &PDPTWData::getClosestLocationsID(int id) const
{
    // sanity check
    if (id < 1 || id > getLocationCount())
    {
        spdlog::error("getClosestLocationsID: Location ID {} is out of valid range [1, {}]", id, getLocationCount());
        throw std::out_of_range("Error: Location ID is out of valid range [1, " + std::to_string(getLocationCount()) +
                                "]");
    }
    return closestLocations.at(id - 1);// vector indexed from 0
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
