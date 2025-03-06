#include "insert_route.h"


InsertRoute::InsertRoute() = default;

void InsertRoute::modifySolution(Solution &solution)
{
    std::vector<Route> &routes = solution.getRoutes();
    routes.emplace_back();
}

double InsertRoute::evaluate(Solution const &solution) const
{
    return 0;
}

int InsertRoute::getAddedPairs() const
{
    return -1;
}

ModificationCheckVariant InsertRoute::asCheckVariant() const
{
    return *this;
}
