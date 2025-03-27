#include "clean_empty_route.h"
#include "lns/modification/route/remove_route.h"

CleanEmptyRoute::CleanEmptyRoute() {}

void CleanEmptyRoute::destroySolution(Solution &solution) const
{
    std::cout << "Clean ";
    for (int routeIndex = solution.getRoutes().size() - 1; routeIndex >= 0; --routeIndex)
    {
        if (solution.getRoute(routeIndex).getRoute().empty())
        {
            RemoveRoute remRoute = RemoveRoute(routeIndex);
            solution.applyDestructSolution(remRoute);
        }
    }
}