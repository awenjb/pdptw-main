#include "enumerate.h"

namespace enumeration
{
    /**
     * Enumerate InsertPair modifications.
     * consumeModification is called for each modification.
     * Does some checks to cut some loops. (TO DO)
     * @param solution
     * @param pair
     * @param consumeModification called when a modification is created
     */
    void enumerateAllInsertPair(Solution const &solution, Pair const &pair,
                                std::function<void(InsertPair &&)> const &consumeModification)
    {
        int routeIndex = 0;
        // Insert into existing routes
        for (Route const &route: solution.getRoutes())
        {
            for (int p = 0; p <= route.getSize(); p++)
            {
                for (int d = p; d <= route.getSize(); d++)
                {
                    Index index = std::make_tuple(routeIndex, p, d);
                    //std::cout << std::get<0>(index) << " " <<  std::get<1>(index) << " " << std::get<2>(index) << "\n";
                    consumeModification(InsertPair(index, pair));
                }
            }
            ++routeIndex;
        }
    }
}// namespace enumeration