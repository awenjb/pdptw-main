#include "random_destroy.h"
#include "lns/solution/solution.h"
#include "utils.h"
#include "types.h"

#include "lns/modification/pair/remove_pair.h"

#include <algorithm> 

void RandomDestroy::destroySolution(Solution &solution) const
{
    std::cout << "RD ";

    int nbRequests = solution.requestsFulFilledCount();
    int actualNumberOfPairsToDestroy = std::min(nbRequests, numberOfPairsToDestroy);
    int remainingPairToDelete = actualNumberOfPairsToDestroy;

    while (remainingPairToDelete > 0)
    {
        // too complicated
        // Other (simpler) option -> choose random route then choose random pair ? (does not respect equiprobability)
     
        // choose a random location
        int locationNumber = util::getRandomInt(0, remainingPairToDelete * 2 - 1);

        int pairID = 0;
        int routeID = 0;
        int position = 0;
        Index index = std::make_tuple(0,0,0);
        
        // retrieve index (route and position)
        for (const Route &route : solution.getRoutes())
        {
            position += route.getSize();
            if (locationNumber < position)
            {
                // calculate the location position
                position = route.getSize() - (position - locationNumber);

                // get location associated to the position
                Location loc = solution.getData().getLocation(route.getRoute().at(position));

                // retrieve pickupID (= PairID)
                if (loc.getLocType() == LocType::PICKUP )
                {
                    pairID = route.getRoute().at(position);
                    std::get<1>(index) = position;
                    std::get<2>(index) = route.getPairLocationPosition(position, solution.getData());
                }
                else
                {
                    pairID = loc.getPair();
                    std::get<1>(index) = route.getPairLocationPosition(position, solution.getData());
                    std::get<2>(index) = position;
                }

                std::get<0>(index) = routeID;

                break;
            }
            ++routeID;
        }
        
        RemovePair remPair = RemovePair(index, solution.getData().getPair(pairID));

        solution.applyDestructSolution(remPair);

        // update les compteurs
        --remainingPairToDelete;
    }
}