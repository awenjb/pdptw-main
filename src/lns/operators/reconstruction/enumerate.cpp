#include "enumerate.h"

namespace enumeration
{
    /**
     * Enumerate InsertPair modifications.
     * Does some checks to cut some loops. (TO DO)
     * @param solution
     * @param pair
     * @param ModificationContainer
     */
    void enumerateAllInsertPair(Solution const &solution, Pair const &pair, std::function<void(InsertPair &&)> const &consumeModification)
    {
        int routeIndex = 0;
        // Insert into existing routes
        for (Route const &route: solution.getRoutes())
        {
            int routeSize = route.getSize();
            for (int p = 0; p <= routeSize; ++p)
            {
                for (int d = p; d <= routeSize; ++d)
                {
                    Index index = std::make_tuple(routeIndex, p, d);

                    consumeModification(InsertPair(index,pair));

                    // std::unique_ptr<InsertPair> modification = std::make_unique<InsertPair>(index, pair);
                    // // add to list if valid modification
                    // if (solution.checkModification(*modification))
                    // {
                    //     std::cout << " => Insert Modification\n";
                    //     list.push_front(std::move(modification));
                    // }
                }
            }
            ++routeIndex;
        }
    }
}// namespace enumeration