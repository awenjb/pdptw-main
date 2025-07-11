#include "enumerate.h"

#include "input/data.h"
#include "input/pdptw_data.h"
#include "lns/constraints/constraint.h"
#include "utils.h"

#include <algorithm>
#include <vector>

namespace enumeration
{
    /**
     * Enumerate InsertPair modifications (no cuts).
     * @param solution
     * @param pair
     * @param ModificationContainer
     */
    void enumerateAllInsertPair(Solution const &solution, Pair const &pair,
                                std::function<void(InsertPair &&)> const &consumeModification)
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

                    consumeModification(InsertPair(index, pair));
                }
            }
            ++routeIndex;
        }
    }

    /**
     * Enumerate InsertPair modifications.
     * @param solution
     * @param pair
     * @param bestModificationPtr
     * @param bestCost
     * @param blinkRate
     */
    void enumerateAllInsertPairOpti(Solution const &solution, Pair const &pair,
                                    std::unique_ptr<AtomicRecreation> &bestModificationPtr, double &bestCost,
                                    double blinkRate)
    {
        int routeIndex = 0;
        PDPTWData const &data = solution.getData();

        // try to insert into all existing routes
        for (Route const &route: solution.getRoutes())
        {
            int routeSize = route.getSize();
            std::vector<int> const &routeIDs = route.getRoute();

            // for every pickup position
            for (int p = 0; p <= routeSize; ++p)
            {
                // compute pickup cost
                int prevPickup = (p == 0) ? 0 : routeIDs.at(p - 1);
                int nextPickup = (p >= routeSize) ? 0 : routeIDs.at(p);
                double pickupCost = data::addedCostForInsertion(data, prevPickup, pair.getPickup().getId(), nextPickup);

                // cut if the pickupCost is not cheaper than the best known insertion cost
                if (pickupCost >= bestCost)
                {
                    continue;
                }

                // for every delivery position
                for (int d = p; d <= routeSize; ++d)
                {
                    // compute delivery cost
                    int prevDelivery = (d == 0) ? 0 : routeIDs.at(d - 1);
                    if (p == d)
                    {
                        prevDelivery = pair.getPickup().getId();
                    }
                    int nextDelivery = (d >= routeIDs.size()) ? 0 : routeIDs.at(d);
                    double deliveryCost =
                            data::addedCostForInsertion(data, prevDelivery, pair.getDelivery().getId(), nextDelivery);

                    double cost = pickupCost + deliveryCost;

                    Index index = std::make_tuple(routeIndex, p, d);

                    InsertPair modification = InsertPair(index, pair);

                    // first test the cost
                    // if the modification is better, then blink,
                    // then check the modification
                    // then store the best cost and the modification to the pointer
                    if (cost < bestCost && util::getRandom() >= blinkRate && solution.checkModification(modification))
                    {
                        bestModificationPtr = std::make_unique<InsertPair>(modification);
                        bestCost = cost;
                    }
                }
            }
            ++routeIndex;
        }
    }

    void enumerateAllInsertPairLTTKBest(Solution const &solution, Pair const &pair,
                                        std::unique_ptr<AtomicRecreation> &bestModificationPtr, double &bestCost,
                                        double blinkRate, size_t k)
    {
        int routeIndex = 0;
        PDPTWData const &data = solution.getData();
        std::vector<std::pair<double, std::unique_ptr<InsertPair>>> candidates;
        std::vector<std::pair<double, std::unique_ptr<AtomicRecreation>>> bestK;

        bestK.clear();

        for (Route const &route: solution.getRoutes())
        {
            int routeSize = route.getSize();
            std::vector<int> const &routeIDs = route.getRoute();

            for (int p = 0; p <= routeSize; ++p)
            {
                int prevPickup = (p == 0) ? 0 : routeIDs.at(p - 1);
                int nextPickup = (p >= routeSize) ? 0 : routeIDs.at(p);
                double pickupCost = data::addedCostForInsertion(data, prevPickup, pair.getPickup().getId(), nextPickup);

                for (int d = p; d <= routeSize; ++d)
                {
                    int prevDelivery = (d == 0) ? 0 : routeIDs.at(d - 1);
                    if (p == d)
                    {
                        prevDelivery = pair.getPickup().getId();
                    }
                    int nextDelivery = (d >= routeIDs.size()) ? 0 : routeIDs.at(d);
                    double deliveryCost =
                            data::addedCostForInsertion(data, prevDelivery, pair.getDelivery().getId(), nextDelivery);

                    double cost = pickupCost + deliveryCost;
                    Index index = std::make_tuple(routeIndex, p, d);
                    InsertPair modification(index, pair);

                    // this check does not take into account the added travel time due to more weight
                    if (util::getRandom() >= blinkRate && solution.checkModification(modification))
                    {
                        candidates.emplace_back(cost, std::make_unique<InsertPair>(modification));
                    }
                }
            }
            ++routeIndex;
        }

        std::sort(candidates.begin(), candidates.end(), [](auto const &a, auto const &b) { return a.first < b.first; });

        int tested = 0;
        for (auto &[cost, ptr]: candidates)
        {
            if (solution.checkModificationLTT(*ptr))
            {
                bestCost = cost;
                bestModificationPtr = std::move(ptr);
                break; //
                // if (bestK.size() >= k) 
                // {
                //     break;
                // }
            }
            ++tested; //
            if (tested >= k) //
            { //
                break; //
            } //
        }

        // if (!bestK.empty())
        // {
        //     auto bestIt = std::min_element(
        //             bestK.begin(), bestK.end(), [](auto const &a, auto const &b) { return a.first < b.first; });
        //     bestCost = bestIt->first;
        //     bestModificationPtr = std::move(bestIt->second);
        // }
        // else
        // {
        //     // aucun candidat valide trouvé : initialiser avec une valeur par défaut
        //     bestCost = std::numeric_limits<double>::max();
        //     bestModificationPtr = nullptr;
        // }
        if (!bestK.empty())
        {
            bestCost = std::numeric_limits<double>::max();
            bestModificationPtr = nullptr;
        }
    }

}// namespace enumeration
