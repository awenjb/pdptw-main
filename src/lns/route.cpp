#include "route.h"

Route::Route(std::vector<int> route, int cost) : route(route), cost(cost) {}


int Route::getCost() const
{
    return cost;
}

const std::vector<int>& Route::getRoute() const
{
    return route;
}   