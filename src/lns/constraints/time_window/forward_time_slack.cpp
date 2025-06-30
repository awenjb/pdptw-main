#include "forward_time_slack.h"

#include "input/data.h"
#include "input/load_dependent.h"
#include "lns/solution/solution.h"

std::vector<TimeInteger> const &ForwardTimeSlack::getFTS() const
{
    return FTS;
}

std::vector<TimeInteger> const &ForwardTimeSlack::getEarliestArrival() const
{
    return earliestArrival;
}

std::vector<TimeInteger> const &ForwardTimeSlack::getLatestArrival() const
{
    return latestArrival;
}

void ForwardTimeSlack::initFTS(PDPTWData const &data, Route const &route)
{
    int n = route.getSize();
    std::vector<int> const &routeIDs = route.getRoute();

    earliestArrival = std::vector<TimeInteger>(n + 1, 0);
    latestArrival = std::vector<TimeInteger>(n + 1, 0);
    FTS = std::vector<TimeInteger>(n + 1, 0);

    // Compute arrival times
    double depotStart = data.getDepot().getTimeWindow().getStart();
    double firstLocationStart = data.getLocation(routeIDs.at(0)).getTimeWindow().getStart();
    double depotToFirst = data::travelCost(data, 0, routeIDs.at(0));

    earliestArrival.at(0) = std::max(firstLocationStart, depotStart + depotToFirst);

    for (int i = 1; i < n; ++i)
    {
        double start = data.getLocation(routeIDs.at(i)).getTimeWindow().getStart();
        double service = data.getLocation(routeIDs.at(i - 1)).getServiceDuration();
        double travelTime = data::travelCost(data, routeIDs.at(i - 1), routeIDs.at(i));

        earliestArrival.at(i) = std::max(start, earliestArrival.at(i - 1) + service + travelTime);
    }

    // Compute earliest arrival at depot after last location
    double lastService = data.getLocation(routeIDs.at(n - 1)).getServiceDuration();
    double lastToDepot = data::travelCost(data, routeIDs.at(n - 1), 0);
    earliestArrival.at(n) = earliestArrival.at(n - 1) + lastService + lastToDepot;

    // Compute latest arrival times
    latestArrival.at(n - 1) = data.getLocation(routeIDs.at(n - 1)).getTimeWindow().getEnd();

    for (int i = n - 2; i >= 0; --i)
    {
        double end = data.getLocation(routeIDs.at(i)).getTimeWindow().getEnd();
        double service = data.getLocation(routeIDs.at(i)).getServiceDuration();
        double travelTime = data::travelCost(data, routeIDs.at(i), routeIDs.at(i + 1));

        latestArrival.at(i) = std::min(latestArrival.at(i + 1) - service - travelTime, end);
    }

    // latest arrival at depot
    latestArrival.at(n) = data.getDepot().getTimeWindow().getEnd();

    // Compute FTS
    for (int i = 0; i <= n; ++i)
    {
        FTS.at(i) = latestArrival.at(i) - earliestArrival.at(i);
    }
}

bool ForwardTimeSlack::isPickupDeliveryInsertionValid(PDPTWData const &data, Route const &route, int pickupID,
                                                      int deliveryID, int insertPickupIndex,
                                                      int insertDeliveryIndex) const
{
    std::vector<int> const &routeIDs = route.getRoute();
    int n = route.getSize();

    // safety
    if (insertPickupIndex > insertDeliveryIndex)
    {
        return false;
    }

    // if the route is empty, only check de time window
    if (routeIDs.empty())
    {
        double startTW = data.getLocation(pickupID).getTimeWindow().getStart();
        double endTW = data.getLocation(deliveryID).getTimeWindow().getEnd();

        return (insertPickupIndex == 0) && (insertDeliveryIndex == 0) &&
               (!(startTW > data.getDepot().getTimeWindow().getEnd() ||
                  endTW < data.getDepot().getTimeWindow().getStart()));
    }


    // Algo

    // Pickup
    // two specific cases, insertion in first or last
    int prev = (insertPickupIndex == 0) ? 0 : routeIDs.at(insertPickupIndex - 1);
    int next = (insertPickupIndex == n) ? 0 : routeIDs.at(insertPickupIndex);

    double arrivalPrev =
            (prev == 0) ? data.getDepot().getTimeWindow().getStart() : getEarliestArrival().at(insertPickupIndex - 1);
    double travelToNew = data::travelCost(data, prev, pickupID);
    double serviceNew = data.getLocation(pickupID).getServiceDuration();
    double travelNewToNext = data::travelCost(data, pickupID, next);

    double newArrival = std::max(data.getLocation(pickupID).getTimeWindow().getStart(),
                                 arrivalPrev + data.getLocation(prev).getServiceDuration() + travelToNew);


    // Check TW
    if (!(data.getLocation(pickupID).getTimeWindow().isValid(newArrival)))
    {
        return false;
    }

    // Calculate the pickup duration
    double arrivalNext =
            std::max(data.getLocation(next).getTimeWindow().getStart(), newArrival + serviceNew + travelNewToNext);

    double pickupDuration = arrivalNext - earliestArrival.at(insertPickupIndex);
    // Check FTS
    if (/*next != 0 &&*/ pickupDuration >= getFTS().at(insertPickupIndex))
    {
        return false;
    }

    // Delivery
    // if insertDeliveryIndex == insertPickupIndex, then Pickup is just before Delivery in the route
    int prevDelivery = (insertDeliveryIndex == insertPickupIndex) ? pickupID : routeIDs.at(insertDeliveryIndex - 1);
    int nextDelivery = (insertDeliveryIndex >= n) ? 0 : routeIDs.at(insertDeliveryIndex);

    // We have to take into account the duration of the pickup
    // (previous earliest arrival) + (time taken by the pickup) - (time of traveling between prev and next)
    double arrivalPrevDelivery =
            (prevDelivery == pickupID) ? newArrival : getEarliestArrival().at(insertDeliveryIndex - 1) + pickupDuration;

    double travelToDelivery = data::travelCost(data, prevDelivery, deliveryID);
    double serviceDelivery = data.getLocation(deliveryID).getServiceDuration();
    double travelDeliveryToNext = data::travelCost(data, deliveryID, nextDelivery);
    double newArrivalDelivery =
            std::max(data.getLocation(deliveryID).getTimeWindow().getStart(),
                     arrivalPrevDelivery + data.getLocation(prevDelivery).getServiceDuration() + travelToDelivery);

    // Check TW
    if (!(data.getLocation(deliveryID).getTimeWindow().isValid(newArrivalDelivery)))
    {
        return false;
    }

    // Calculate the delivery duration
    double arrivalNextDelivery = std::max(data.getLocation(nextDelivery).getTimeWindow().getStart(),
                                          newArrivalDelivery + serviceDelivery + travelDeliveryToNext);
    double deliveryDuration = arrivalNextDelivery - earliestArrival.at(insertDeliveryIndex);

    // Check FTS validity
    if (/*nextDelivery != 0 &&*/ deliveryDuration >= getFTS().at(insertDeliveryIndex))
    {
        return false;
    }

    return true;
}

// TO DO some cuts in the loop to avoid recalculate everything
void ForwardTimeSlack::updateFTSAfterInsertion(PDPTWData const &data, Route const &route, int insertPickupIndex,
                                               int insertDeliveryIndex)
{
    int n = route.getSize();
    std::vector<int> const &routeIDs = route.getRoute();

    // Resize vectors to match the updated route size
    earliestArrival.resize(n + 1, 0);
    latestArrival.resize(n + 1, 0);
    FTS.resize(n + 1, 0);

    // Recalculate earliest arrival times
    for (int i = insertPickupIndex; i <= n; ++i)
    {
        if (i == 0)// specific case for the first element
        {
            double depotStart = data.getDepot().getTimeWindow().getStart();
            double firstLocationStart = data.getLocation(routeIDs.at(0)).getTimeWindow().getStart();
            double depotToFirst = data::travelCost(data, 0, routeIDs.at(0));
            earliestArrival.at(0) = std::max(firstLocationStart, depotStart + depotToFirst);
        }
        else
        {
            int prev = routeIDs.at(i - 1);
            int curr = (i == n) ? 0 : routeIDs.at(i);// specific case for the last element
            double start = data.getLocation(curr).getTimeWindow().getStart();
            double service = data.getLocation(prev).getServiceDuration();
            double travelTime = data::travelCost(data, prev, curr);

            earliestArrival.at(i) = std::max(start, earliestArrival.at(i - 1) + service + travelTime);
        }
    }

    // try add element in front then recalculate from the delivery index
    // Recalculate latest arrival times
    latestArrival.at(n) = data.getDepot().getTimeWindow().getEnd();
    latestArrival.at(n - 1) = data.getLocation(routeIDs.at(n - 1)).getTimeWindow().getEnd();
    for (int i = n - 2; i >= 0; --i)
    {
        double end = data.getLocation(routeIDs.at(i)).getTimeWindow().getEnd();
        double service = data.getLocation(routeIDs.at(i)).getServiceDuration();
        double travelTime = data::travelCost(data, routeIDs.at(i), routeIDs.at(i + 1));

        latestArrival.at(i) = std::min(latestArrival.at(i + 1) - service - travelTime, end);
    }

    // Recalculate FTS
    for (int i = 0; i <= n; ++i)
    {
        FTS.at(i) = latestArrival.at(i) - earliestArrival.at(i);
    }
}

// TO DO some cuts in the loop to avoid recalculate everything
void ForwardTimeSlack::updateFTSAfterDeletion(PDPTWData const &data, Route const &route, int removePickupIndex,
                                              int removeDeliveryIndex)
{
    int n = route.getSize();
    std::vector<int> const &routeIDs = route.getRoute();

    // empty route
    if (n == 0)
    {
        earliestArrival.clear();
        latestArrival.clear();
        FTS.clear();
        return;
    }

    // Resize vectors to match the updated route size
    earliestArrival.resize(n + 1, 0);
    latestArrival.resize(n + 1, 0);
    FTS.resize(n + 1, 0);

    // Recalculate earliest arrival times
    for (int i = 0; i <= n; ++i)
    {
        if (i == 0)// Specific case for the first element
        {
            double depotStart = data.getDepot().getTimeWindow().getStart();
            double firstLocationStart = data.getLocation(routeIDs.at(0)).getTimeWindow().getStart();
            double depotToFirst = data::travelCost(data, 0, routeIDs.at(0));
            earliestArrival.at(0) = std::max(firstLocationStart, depotStart + depotToFirst);
        }
        else
        {
            int prev = routeIDs.at(i - 1);
            int curr = (i == n) ? 0 : routeIDs.at(i);
            double start = data.getLocation(curr).getTimeWindow().getStart();
            double service = data.getLocation(prev).getServiceDuration();
            double travelTime = data::travelCost(data, prev, curr);

            earliestArrival.at(i) = std::max(start, earliestArrival.at(i - 1) + service + travelTime);
        }
    }

    // Recalculate latest arrival times
    latestArrival.at(n) = data.getDepot().getTimeWindow().getEnd();
    latestArrival.at(n - 1) = data.getLocation(routeIDs.at(n - 1)).getTimeWindow().getEnd();
    for (int i = n - 2; i >= 0; --i)
    {
        double end = data.getLocation(routeIDs.at(i)).getTimeWindow().getEnd();
        double service = data.getLocation(routeIDs.at(i)).getServiceDuration();
        double travelTime = data::travelCost(data, routeIDs.at(i), routeIDs.at(i + 1));

        latestArrival.at(i) = std::min(latestArrival.at(i + 1) - service - travelTime, end);
    }

    // Recalculate FTS
    for (int i = 0; i <= n; ++i)
    {
        FTS.at(i) = latestArrival.at(i) - earliestArrival.at(i);
    }
}

void ForwardTimeSlack::updateFTSAfterInsertionLTT(PDPTWData const &data, Route const &route, int insertPickupIndex,
                                                  int insertDeliveryIndex)
{
    int n = route.getSize();
    std::vector<int> const &routeIDs = route.getRoute();

    earliestArrival.resize(n + 1, 0);
    latestArrival.resize(n + 1, 0);
    FTS.resize(n + 1, 0);

    // Forward pass — compute earliestArrival using load-dependent travel time
    double load = 0;
    for (int i = 0; i <= n; ++i)
    {
        if (i == 0)
        {
            int first = routeIDs.at(0);
            double depotStart = data.getDepot().getTimeWindow().getStart();
            double firstTWStart = data.getLocation(first).getTimeWindow().getStart();
            double travelTime = ltt::getTravelTimeLTT(data, load, 0, first);
            earliestArrival.at(0) = std::max(firstTWStart, depotStart + travelTime);
            load += data.getLocation(first).getDemand();
        }
        else
        {
            int prev = routeIDs.at(i - 1);
            int curr = (i == n) ? 0 : routeIDs.at(i);// return to depot if end
            double service = data.getLocation(prev).getServiceDuration();

            
            double travelTime = ltt::getTravelTimeLTT(data, load, prev, curr);
            // double travelTime = data.getMatrix().at(prev).at(curr) / 6.94444;
            
            double startTW = data.getLocation(curr).getTimeWindow().getStart();
            earliestArrival.at(i) = std::max(startTW, earliestArrival.at(i - 1) + service + travelTime);
            if (i < n)
            {
                load += data.getLocation(curr).getDemand();
            }
        }
    }

    // Backward pass — compute latestArrival
    latestArrival.at(n) = data.getDepot().getTimeWindow().getEnd();
    double loadBack = 0;
    latestArrival.at(n - 1) = data.getLocation(routeIDs.at(n - 1)).getTimeWindow().getEnd();
    loadBack += data.getLocation(routeIDs.at(n - 1)).getDemand();
    for (int i = n - 2; i >= 0; --i)
    {
        int curr = routeIDs.at(i);
        int next = routeIDs.at(i + 1);
        double service = data.getLocation(curr).getServiceDuration();


        double travelTime = ltt::getTravelTimeLTT(data, loadBack, curr, next);
        // double travelTime = data.getMatrix().at(curr).at(next) / 6.94444;
        
        
        double endTW = data.getLocation(curr).getTimeWindow().getEnd();

        latestArrival.at(i) = std::min(latestArrival.at(i + 1) - service - travelTime, endTW);
        loadBack += data.getLocation(curr).getDemand();
    }

    // FTS
    for (int i = 0; i <= n; ++i)
    {
        FTS.at(i) = latestArrival.at(i) - earliestArrival.at(i);
    }
}

void ForwardTimeSlack::updateFTSAfterDeletionLTT(PDPTWData const &data, Route const &route, int removePickupIndex,
                                                 int removeDeliveryIndex)
{
    int n = route.getSize();
    std::vector<int> const &routeIDs = route.getRoute();

    if (n == 0)
    {
        earliestArrival.clear();
        latestArrival.clear();
        FTS.clear();
        return;
    }

    earliestArrival.resize(n + 1, 0);
    latestArrival.resize(n + 1, 0);
    FTS.resize(n + 1, 0);

    // Forward pass — compute earliestArrival using load-dependent travel time
    double load = 0;
    for (int i = 0; i <= n; ++i)
    {
        if (i == 0)
        {
            int first = routeIDs.at(0);
            double depotStart = data.getDepot().getTimeWindow().getStart();
            double firstTWStart = data.getLocation(first).getTimeWindow().getStart();


            double travelTime = ltt::getTravelTimeLTT(data, load, 0, first);
            // double travelTime = data.getMatrix().at(0).at(first) / 6.94444;

            earliestArrival.at(0) = std::max(firstTWStart, depotStart + travelTime);
            load += data.getLocation(first).getDemand();
        }
        else
        {
            int prev = routeIDs.at(i - 1);
            int curr = (i == n) ? 0 : routeIDs.at(i);// return to depot if end
            double service = data.getLocation(prev).getServiceDuration();


            double travelTime = ltt::getTravelTimeLTT(data, load, prev, curr);
            // double travelTime = data.getMatrix().at(0).at(first) / 6.94444;

            double startTW = data.getLocation(curr).getTimeWindow().getStart();
            earliestArrival.at(i) = std::max(startTW, earliestArrival.at(i - 1) + service + travelTime);
            if (i < n)
            {
                load += data.getLocation(curr).getDemand();
            }
        }
    }

    // Backward pass — compute latestArrival using load-dependent travel time
    latestArrival.at(n) = data.getDepot().getTimeWindow().getEnd();

    double loadBack = 0;
    latestArrival.at(n - 1) = data.getLocation(routeIDs.at(n - 1)).getTimeWindow().getEnd();
    loadBack += data.getLocation(routeIDs.at(n - 1)).getDemand();

    for (int i = n - 2; i >= 0; --i)
    {
        int curr = routeIDs.at(i);
        int next = routeIDs.at(i + 1);
        double service = data.getLocation(curr).getServiceDuration();
        double travelTime = ltt::getTravelTimeLTT(data, loadBack, curr, next);
        double endTW = data.getLocation(curr).getTimeWindow().getEnd();

        latestArrival.at(i) = std::min(latestArrival.at(i + 1) - service - travelTime, endTW);
        loadBack += data.getLocation(curr).getDemand();
    }

    // Compute forward time slack
    for (int i = 0; i <= n; ++i)
    {
        FTS.at(i) = latestArrival.at(i) - earliestArrival.at(i);
    }
}

void ForwardTimeSlack::print() const
{
    for (TimeInteger timeVal: earliestArrival)
    {
        std::cout << timeVal << " ";
    }
    std::cout << std::endl;
    for (TimeInteger timeVal: latestArrival)
    {
        std::cout << timeVal << " ";
    }
    std::cout << std::endl;
    for (TimeInteger ftsVal: FTS)
    {
        std::cout << ftsVal << " ";
    }
}