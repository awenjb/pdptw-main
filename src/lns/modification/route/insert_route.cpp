#include "insert_route.h"


InsertRoute::InsertRoute() {}

void InsertRoute::modifySolution(Solution &solution)
{
    std::vector<Route> &routes = solution.getRoutes();
    routes.push_back(Route());
}

double InsertRoute::evaluate(Solution const &solution) const
{
    return 0;
}

Location const *InsertRoute::getAddedLocation() const
{
    return nullptr;
}

