#include "split_string_removal.h"

#include "input/location.h"
#include "input/pair.h"
#include "input/pdptw_data.h"
#include "lns/modification/pair/remove_pair.h"
#include "utils.h"

#include <vector>
#include <numeric>
#include <algorithm>

namespace sisr
{

    double computeAverageCardinalitySplit(std::vector<Route> const &routes)
    {
        if (routes.empty())
        {
            return 0.0;
        }
        return std::accumulate(routes.begin(),
                               routes.end(),
                               0.0,
                               [](double sum, Route const &route) { return sum + route.getSize(); }) /
               routes.size();
    }

    /**
     * Remove a string in the solution while preserving a consecutive subsequence inside it.
     * @param routeIndex The route on which the removal happens
     * @param stringLength Total number of requests targeted
     * @param preserveLength Length of the consecutive subsequence to preserve inside the string
     * @param startLocation A location used as a center point
     */
    void removeAroundSubsequenceSplit(Solution &solution, int routeIndex, unsigned int stringLength, unsigned int preserveLength, int startLocation)
    {
        if (preserveLength >= stringLength) {
            return; // nothing to remove
        }

        std::vector<int> toRemove;
        toRemove.reserve(stringLength - preserveLength);

        PDPTWData const &data = solution.getData();
        Route const &route = solution.getRoute(routeIndex);
        std::vector<int> const &routeLocationIDs = route.getRoute();

        int startPosition = route.getIndex(startLocation);
        if (startPosition == -1) return;

        int fullLeft = std::max(0, startPosition - ((int) stringLength / 2));
        int fullRight = fullLeft + (int) stringLength - 1;

        if (fullRight >= (int)routeLocationIDs.size()) {
            fullRight = routeLocationIDs.size() - 1;
            fullLeft = std::max(0, fullRight - (int)stringLength + 1);
        }

        int preservedLeft = fullLeft + util::getRandomInt(0, stringLength - preserveLength);
        int preservedRight = preservedLeft + preserveLength - 1;

        for (int i = fullLeft; i <= fullRight; ++i)
        {
            if (i >= preservedLeft && i <= preservedRight) continue;

            int locID = routeLocationIDs.at(i);
            Location const &loc = data.getLocation(locID);

            int pairID = (loc.getLocType() == LocType::PICKUP) ? locID : loc.getPair();

            if (std::find(toRemove.begin(), toRemove.end(), pairID) == toRemove.end())
            {
                toRemove.emplace_back(pairID);
            }
        }

        for (int pairID: toRemove)
        {
            int position = solution.getRoute(routeIndex).getIndex(pairID);
            Index index = std::make_tuple(routeIndex, position, route.getPairLocationPosition(position, data));
            Pair const &pair = data.getPair(pairID);

            RemovePair removePair(index, pair);
            solution.applyDestructSolution(removePair);
        }
    }

    void SISRsRuinSplit(Solution &solution, unsigned int maxStringSize, unsigned int averageNumberRemovedElement)
    {
        auto averageRouteCardinality = static_cast<unsigned int>(computeAverageCardinalitySplit(solution.getRoutes()));
        unsigned int maxSizeOfString = std::min(maxStringSize, averageRouteCardinality);
        unsigned int maxNumberOfString = (4 * averageNumberRemovedElement) / (1 + maxSizeOfString) - 1;
        unsigned int numberOfString = util::getRandomInt(1, maxNumberOfString + 1);

        int locationSeed = util::getRandomInt(1, solution.getData().getLocationCount());
        std::vector<int> routeIndexUsed;
        routeIndexUsed.reserve(numberOfString);

        constexpr double beta = 0.01; // Split depth as per the paper

        for (int neighbor: solution.getData().getClosestLocationsID(locationSeed))
        {
            int routeIndex = solution.getRouteIDOf(neighbor);
            if (routeIndex != -1 && std::ranges::find(routeIndexUsed, routeIndex) == routeIndexUsed.end())
            {
                unsigned int maxSizeOfThisString =
                    std::min(static_cast<int>(maxSizeOfString), solution.getRoute(routeIndex).getSize());

                if (maxSizeOfThisString < 2)
                    continue; // Must be at least 2 to allow preservation

                unsigned int actualSizeOfThisString = util::getRandomInt(2, maxSizeOfThisString);

                // Dynamically determine `m` = preserveLength using beta
                unsigned int preserveLength = 1;
                unsigned int m_max = std::max(1u, actualSizeOfThisString - 1);
                while (preserveLength < m_max && util::getRandomDouble(0.0, 1.0) < beta) {
                    preserveLength++;
                }

                removeAroundSubsequenceSplit(solution, routeIndex, actualSizeOfThisString, preserveLength, neighbor);
                routeIndexUsed.emplace_back(routeIndex);

                if (routeIndexUsed.size() >= numberOfString)
                    break;
            }
        }
    }

} // namespace

void SplitStringRemoval::destroySolution(Solution &solution) const
{
    sisr::SISRsRuinSplit(solution, maxCardinalityOfString, averageNumberRemovedElement);
}
