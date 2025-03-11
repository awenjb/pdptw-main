#include "enumerate.h"

namespace enumeration
{

    template<std::derived_from<AtomicRecreation> ModificationType>
    void addToListIfValidTemplate(Solution const &solution, ModificationType const &modification, ModificationContainer &list)
    {
        if (solution.checkModification(modification))
        {
            std::cout << " => Insert Modification" << "\n";
            list.push_front(std::make_unique<ModificationType>(modification));
        }
    }

    /**
     * Enumerate InsertPair modifications.
     * Does some checks to cut some loops. (TO DO)
     * @param solution
     * @param pair
     * @param ModificationContainer
     */
    void enumerateAllInsertPair(Solution const &solution, Pair const &pair, ModificationContainer &list)
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
                    // add to list if valid modification
                    enumeration::addToListIfValidTemplate(solution, InsertPair(index, pair), list);
                }
            }
            ++routeIndex;
        }
    }
}// namespace enumeration