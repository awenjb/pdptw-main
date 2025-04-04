#include "bank_focus_string_removal.h"
#include "lns/modification/route/remove_route.h"
#include "lns/operators/destruction/clean_empty_route.h"
#include "utils.h"


namespace
{
    void BankFocusSISRsRuin(Solution &solution, unsigned int maxStringSize, unsigned int averageNumberRemovedElement)
    {
        // |t∈T| average number of location by route
        auto averageRouteCardinality = static_cast<unsigned int>(sisr::computeAverageCardinality(solution.getRoutes()));

        unsigned int maxSizeOfString = std::min(maxStringSize, averageRouteCardinality);               // (5) lmax_s
        unsigned int maxNumberOfString = (4 * averageNumberRemovedElement) / (1 + maxSizeOfString) - 1;// (6) kmax_s
        unsigned int numberOfString = util::getRandomInt(1, maxNumberOfString + 1);                    // (7) k_s


        // select a random location in the bank
        std::vector<int> const &bank = solution.getBank();
        int locationSeed = bank.at(util::getRandomInt(1, bank.size()) - 1);

        // store the index of routes where a string was removed
        std::vector<int> routeIndexUsed;
        routeIndexUsed.reserve(numberOfString);

        // getClosestLocationsID returns the list of all location sorted from the closest to furthest
        for (int neighbor: solution.getData().getClosestLocationsID(locationSeed))
        {
            // recover the routeIndex associated to the locationID, returns -1 if in the bank
            int routeIndex = solution.getRouteIDOf(neighbor);

            if (routeIndex != -1 && std::ranges::find(routeIndexUsed, routeIndex) == routeIndexUsed.end())
            {
                // (8) lmax_t
                unsigned int maxSizeOfThisString =
                        std::min(static_cast<int>(maxSizeOfString), solution.getRoute(routeIndex).getSize());
                // (9) l_t
                unsigned int actualSizeOfThisString = util::getRandomInt(1, maxSizeOfThisString);

                sisr::removeString(solution, routeIndex, actualSizeOfThisString, neighbor);
                routeIndexUsed.emplace_back(routeIndex);

                if (routeIndexUsed.size() >= numberOfString)
                {
                    break;
                }
            }
        }
    }
}// namespace

void BankFocusStringRemoval::destroySolution(Solution &solution) const
{
    // if the bank is empty (all requests are fullfilled), remove a route
    if (solution.getBank().empty())
    {
        // clean empty routes
        CleanEmptyRoute clean = CleanEmptyRoute();
        clean.destroySolution(solution);

        // maybe try a heuristic to remove a specific route (smallest, longest, etc...)
        RemoveRoute remove = RemoveRoute(util::getRandomInt(1, solution.getRoutes().size()) - 1);
        solution.applyDestructSolution(remove);
    }
    std::cout << "After preparation :" << std::endl;
    solution.print();
    BankFocusSISRsRuin(solution, maxCardinalityOfString, averageNumberRemovedElement);
}