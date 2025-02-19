#include "route.h"
#include <iostream>

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