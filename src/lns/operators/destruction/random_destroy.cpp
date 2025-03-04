#include "random_destroy.h"
#include "lns/solution/solution.h"
#include "utils.h"
#include "types.h"

#include "lns/modification/pair/remove_pair.h"

#include <algorithm> 

void RandomDestroy::destroySolution(Solution &solution) const
{

    int nbRequests = solution.requestsFulFilledCount();
    int actualNumberOfPairsToDestroy = std::min(nbRequests, numberOfPairsToDestroy);
    int remainingPairToDelete = actualNumberOfPairsToDestroy;

    while (remainingPairToDelete < 0)
    {
        // too complicated
        // Other (simpler) option -> choose random route then choose random pair ?
     
        // choose a random pair
        int pairNumber = util::getRandomInt(0, remainingPairToDelete * 2 - 1);
        
        int pairID = 0;
        int routeID = 0;
        int count = 0;
        Index index = std::make_tuple(0,0,0);
        
        // retrieve index (route and position)
        for (const Route &route : solution.getRoutes())
        {
            count +=  route.getSize();
            if (pairNumber < count)
            {
                count = count - pairNumber;

                std::get<0>(index) = routeID;
                std::get<1>(index) = count;
                std::get<2>(index) = route.getPairLocationPosition(count, solution.getData());

                // retrieve pickupID (= PairID)
                if (solution.getData().getLocation(route.getRoute().at(count)).getLocType() == LocType::PICKUP )
                {
                    pairID = route.getRoute().at(count);
                }
                else
                {
                    pairID = route.getRoute().at(std::get<2>(index));
                }
                break;
            }
            ++routeID;
        }

        RemovePair remPair = RemovePair(index, solution.getData().getPair(pairID));

        // appliquer la modif (via solution method -> update correctement la solution)
        solution.applyDestructSolution(remPair);

        // update les compteurs
        --remainingPairToDelete;
    }
}