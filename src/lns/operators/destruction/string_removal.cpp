#include "string_removal.h"

#include "input/location.h"
#include "input/pair.h"
#include "input/pdptw_data.h"
#include "lns/modification/pair/remove_pair.h"
#include "utils.h"

#include <vector>

namespace
{

    double computeAverageCardinality(std::vector<Route> const &routes)
    {
        if (routes.empty())
        {
            return 0;
        }
        int total_cardinality = 0;
        for (Route const &route: routes)
        {
            total_cardinality += route.getSize();
        }

        return static_cast<double>(total_cardinality) / routes.size();
    }

    /**
     *  Compute equations (8) and (9) from the SISR paper.
     *  (8) Maximum cardinality of a string
     *  (9) Cardinality of string to be removed
     *  @return pair of (global maximum of string size, Cardinality of the string to be removed)
     */
    std::pair<unsigned int, unsigned int> computeStringsData(unsigned int maxStringSize, unsigned int routeSize)
    {
        // (8) "The maximum cardinality of each string is limited by the initial limit lmax_s and the tour’s cardinality |t|"
        unsigned int maxSizeOfString = std::min(maxStringSize, routeSize);

        // (9) "the cardinality for the string to be removed from tour t is obtained by rounding down a real value selected from U(Lmin, lmax_t+1)"
        unsigned int actualSizeOfString = util::getRandomInt(1, maxSizeOfString);

        return {maxSizeOfString, actualSizeOfString};
    }

    // t, l_t, c^*_t
    /**
     * Remove a string in the solution
     * For now it uses startLocation as the center of the string
     * @param pathIndex the path on which the removal happens
     * @param stringLength the number of requests to remove, expect it to be inferior to the number of requests in the path
     * @param startLocation the location found to start the string, does not mean it will the smallest index removed
     */
    void removeString(Solution &solution, int routeIndex, unsigned int stringLength, int startLocation)
    {
        std::cout << "remove string" << std::endl;
        // ID of pair to remove
        std::vector<int> toRemove;
        toRemove.reserve(stringLength);

        PDPTWData const &data = solution.getData();
        Route const &route = solution.getRoute(routeIndex);
        std::vector<int> const &routeLocationIDs = route.getRoute();

        // Position in the route of the first location
        int startPosition = route.getIndex(startLocation);

        // the location is not in a route
        if (startPosition == -1)
        {
            return;
        }

        int left = std::max(0, startPosition - ((int) stringLength / 2));
        int right = left + (int) stringLength - 1;

        // we don't want to go out of bound
        if (right >= routeLocationIDs.size())
        {
            right = routeLocationIDs.size() - 1;
            left = std::max(0, right - (int) stringLength + 1);// keep a number of stringLength elements
        }

        ////////////////////:
        // try : get the left index and remove stringLength times on the left index

        // collect each locationID and store the pairID in the toRemove vector
        for (int i = left; i <= right; ++i)
        {
            // pickupID = pairID
            // if pickup and not already in the list
            if (data.getLocation(routeLocationIDs.at(i)).getLocType() == LocType::PICKUP &&
                std::find(toRemove.begin(), toRemove.end(), routeLocationIDs.at(i)) == toRemove.end())
            {
                toRemove.emplace_back(routeLocationIDs.at(i));
            }
            // or if delivery and pairID not already in the list
            else if (data.getLocation(routeLocationIDs.at(i)).getLocType() == LocType::DELIVERY &&
                     std::find(toRemove.begin(), toRemove.end(), data.getLocation(routeLocationIDs.at(i)).getPair()) ==
                             toRemove.end())
            {
                toRemove.emplace_back(data.getLocation(routeLocationIDs.at(i)).getPair());
            }
        }

        std::cout << std::endl << "PairID (pickup):" << std::endl;
        for (int i: toRemove)
        {
            std::cout << i << " ";
        }
        std::cout << std::endl;

        //remove the collected pairID
        for (int pairID: toRemove)
        {
            // get index and Pair
            int position = solution.getRoute(routeIndex).getIndex(pairID);
            Index index = std::make_tuple(routeIndex, position, route.getPairLocationPosition(position, data));
            Pair const &pair = data.getPair(pairID);
            
            // apply remove
            RemovePair removePair(index, pair);
            solution.applyDestructSolution(removePair);
        }
    }
}// namespace


// Procedure RUIN(s, Lmax, c^bar)
//   1. Compute lmax_s, kmax_s, k_s according to equations (5), (6), and (7)
//   2. cseed_s ← RandomClient(s)
//   3. R ← ∅  // Set of affected tours
//   4. For each client c in Adjacent(cseed_s) Do
//       5. If c ∉ A and |R| < k_s Then
//           6. c*_t ← c
//           7. Compute lmax_t, lt according to equations (8) and (9)
//           8. A ← A ∪ RemoveClient(T, lt, c*_t)
//           9. R ← R ∪ {t}
//       10. End If
//   11. End For
// End Procedure


/**
 *  Ruin procedure from the SISR paper.
 *  c^bar = averageNumberRemovedElement.
 *  L^max = maxStringSize.
 */
void SISRsRuin(Solution &solution, unsigned int maxStringSize, unsigned int averageNumberRemovedElement)
{
    // |t∈T| average number of location by route
    unsigned int averageRouteCardinality = computeAverageCardinality(solution.getRoutes());

    // compute equation (5) (6) and (7) from the SISR paper.
    // (5) lmax_s
    unsigned int maxSizeOfString =
            std::min(static_cast<std::size_t>(maxStringSize), static_cast<std::size_t>(averageRouteCardinality));
    // (6) kmax_s
    unsigned int maxNumberOfString = (4 * averageNumberRemovedElement) / (1 + maxSizeOfString) - 1;
    // (7) k_s
    unsigned int numberOfString = util::getRandomInt(1, maxNumberOfString + 1);

    // get Location ID of the location seed
    int locationSeed = util::getRandomInt(1, solution.getData().getLocationCount());

    // store the index of routes where a string was removed
    std::vector<int> routeIndexUsed;
    routeIndexUsed.reserve(numberOfString);

    std::cout << "locationSeed : " << locationSeed << std::endl;

    // getClosestLocationsID returns the list of all location sorted from the closest to furthest
    for (int neighbor: solution.getData().getClosestLocationsID(locationSeed))
    {
        // recover the routeIndex associated to the locationID, returns -1 if in the bank
        int routeIndex = solution.getRouteIDOf(neighbor);

        // verif de validité
        // -> pas dans pairBank
        // -> route pas déjà modifiée
        if (routeIndex != -1 && std::ranges::find(routeIndexUsed, routeIndex) == routeIndexUsed.end())
        {
            std::cout << "in" << std::endl;

            // (8) lmax_t
            unsigned int maxSizeOfThisString =
                    std::min(static_cast<int>(maxSizeOfString), solution.getRoute(routeIndex).getSize());
            // (9) l_t
            unsigned int actualSizeOfThisString = util::getRandomInt(1, maxSizeOfThisString);

            std::cout << "actualSizeOfThisString " << actualSizeOfThisString << " neighbor " << neighbor << std::endl;

            // string removal
            removeString(solution, routeIndex, actualSizeOfThisString, neighbor);

            // update routeIndexUsed
            routeIndexUsed.emplace_back(routeIndex);

            if (routeIndexUsed.size() >= numberOfString)
            {
                break;
            }
        }
    }
}

// namespace

void StringRemoval::destroySolution(Solution &solution) const
{
    SISRsRuin(solution, maxCardinalityOfString, averageNumberRemovedElement);
}
