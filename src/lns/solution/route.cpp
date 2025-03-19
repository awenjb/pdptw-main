#include "route.h"
#include <iostream>
#include <spdlog/spdlog.h>

Route::Route() = default;
Route::Route(std::vector<int> route, int cost) : route(route), cost(cost) {}


int Route::getCost() const
{
    return cost;
}

const std::vector<int>& Route::getRoute() const
{
    return route;
}   

void Route::print() const
{
    for (const int& id : getRoute())
    {
        std::cout << id << ", ";
    } 
    std::cout << "\n";
}

void Route::insertAt(int locationIndex, int position) 
{
    if (position < 0 || position > route.size()) {
        spdlog::error("Invalid position for the insertion : {}", position);
        throw std::out_of_range("Invalid position for the insertion.");
    }

    route.insert(route.begin() + position, locationIndex);
}


void Route::deleteAt(int position) {
    // Vérification si la position est valide
    if (position < 0 || position >= route.size()) {
        spdlog::error("Invalid position for the suppression : {}", position);
        throw std::out_of_range("Invalid position for the suppression");
    }
    route.erase(route.begin() + position);
}

int Route::getLocation(int index) const
{
    // Vérification si la position est valide
    if (index < 0 || index >= route.size()) {
        spdlog::error("Invalid index when reading route: {}", index);
        throw std::out_of_range("Invalid index when reading route");
    }
    return route.at(index);
}


int Route::getSize() const 
{
    return route.size();
}

int Route::getIndex(int locationID) const 
{
    auto it = std::find(route.begin(), route.end(), locationID);
    if (it != route.end()) {
        return std::distance(route.begin(), it);
    }
    return -1;
}

int Route::getPairLocationPosition(int position, const PDPTWData &data) const
{
    int firstID = getRoute().at(position);
    int secondID = data.getLocation(firstID).getPair();

    for (int i=0; i < getSize(); i++)
    {
        if (getRoute().at(i) == secondID)
        {
            return i;
        }
    }
    
    spdlog::error("Paired location not found for ID {}", firstID);
    throw std::runtime_error("Paired location not found in the route");
}