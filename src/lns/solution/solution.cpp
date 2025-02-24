#include "solution.h"
#include <iostream>
#include <spdlog/spdlog.h>


Solution::Solution(const PDPTWData &data) 
    : data(data), totalCost(0) {

    bank = RequestBank();
    routes = std::vector<Route>();
}


Solution::Solution(const PDPTWData &data, RequestBank bank, std::vector<Route> routes, int totalCost)
    : data(data), bank(bank), routes(routes), totalCost(totalCost) {}

const std::vector<int> & Solution::getBank() const
{
    return bank;
}

const std::vector<Route> & Solution::getRoutes() const
{
    return routes;
}

std::vector<Route> & Solution::getRoutes()
{
    return routes;
}

Route & Solution::getRoute(int routeIndex) 
{
    if (routeIndex < 0 || routeIndex >= routes.size()) 
    {
        spdlog::error("Invalid route index: {}", routeIndex);
        throw std::out_of_range("Invalid route index.");
    }
    return routes[routeIndex];
}

const Route & Solution::getRoute(int routeIndex) const
{
    if (routeIndex < 0 || routeIndex >= routes.size()) 
    {
        spdlog::error("Invalid route index: {}", routeIndex);
        throw std::out_of_range("Invalid route index.");
    }
    return routes[routeIndex];
}

int Solution::getCost()
{
    return totalCost;
}

const PDPTWData & Solution::getData() const
{
    return data;
}

void Solution::print() const
{
    std::cout << "Cost :" << totalCost << "\n" << "Routes : \n";

    for (const Route& id : getRoutes())
    {
        id.print();
    }

    std::cout << "Banques : \n";

    for (const int id : getBank())
    {
        std::cout << id << ", ";
    }
    std::cout << "\n";
}