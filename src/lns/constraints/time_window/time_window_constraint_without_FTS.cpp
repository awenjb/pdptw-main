// #include "input/data.h"
// #include "input/pdptw_data.h"
// #include "input/time_window.h"
// #include "lns/modification/pair/insert_pair.h"
// #include "lns/modification/pair/remove_pair.h"
// #include "lns/modification/route/remove_route.h"
// #include "lns/solution/route.h"
// #include "lns/solution/solution.h"
// #include "time_window_constraint.h"

// #include <iostream>
// #include <iterator>
// #include <vector>

// TimeWindowConstraint::TimeWindowConstraint(Solution const &solution) : Constraint(solution)
// {
//     arrivalTimeContainer.clear();
//     for (Route const &route: solution.getRoutes())
//     {
//         arrivalTimeContainer.emplace_back();
//     }
// }

// TimeWindowConstraint::~TimeWindowConstraint()
// {
//     arrivalTimeContainer.clear();
// }

// std::unique_ptr<Constraint> TimeWindowConstraint::clone(Solution const &newOwningSolution) const
// {
//     std::unique_ptr<TimeWindowConstraint> clonePtr = std::make_unique<TimeWindowConstraint>(newOwningSolution);
//     clonePtr->arrivalTimeContainer = arrivalTimeContainer;
//     return clonePtr;
// }


// bool TimeWindowConstraint::checkInsertion(PDPTWData const &data, Pair const &pair, int routeIndex, int pickupPos,
//                                           int deliveryPos) const
// {
//     auto const &route = getSolution().getRoute(routeIndex);
//     auto const &routeIDs = route.getRoute();
//     auto const &arrivalTimes = arrivalTimeContainer.at(routeIndex);
//     int n = route.getSize();

//     int pickupID = pair.getPickup().getId();
//     int deliveryID = pair.getDelivery().getId();

//     // Empty route
//     if (n == 0)
//     {
//         double arrival = data::travelCost(data, 0, pickupID);
//         if (!data.getLocation(pickupID).getTimeWindow().isValid(arrival))
//         {
//             return false;
//         }

//         double startP = std::max(arrival, data.getLocation(pickupID).getTimeWindow().getStart());
//         double serviceP = data.getLocation(pickupID).getServiceDuration();
//         double travelToD = data::travelCost(data, pickupID, deliveryID);
//         double arrivalD = startP + serviceP + travelToD;

//         if (!data.getLocation(deliveryID).getTimeWindow().isValid(arrivalD))
//         {
//             return false;
//         }

//         return true;
//     }

//     // Simulate the insertion then compute the arrival time

//     std::vector<int> newRoute = routeIDs;
//     newRoute.insert(newRoute.begin() + pickupPos, pickupID);
//     newRoute.insert(newRoute.begin() + deliveryPos + 1, deliveryID);

//     std::vector<double> simulatedArrivals;
//     simulatedArrivals.resize(newRoute.size());

//     double time = 0;
//     int prev = 0;

//     // Calculate new arrival time with the new route

//     for (size_t i = 0; i < newRoute.size(); ++i)
//     {
//         int curr = newRoute.at(i);

//         // travel time
//         time += data::travelCost(data, prev, curr);

//         if (!data.getLocation(curr).getTimeWindow().isValid(time))
//         {
//             return false;
//         }

//         simulatedArrivals.at(i) = time;

//         // service time
//         time = std::max(time, data.getLocation(curr).getTimeWindow().getStart());
//         time += data.getLocation(curr).getServiceDuration();

//         prev = curr;
//     }

//     return true;
// }

// void TimeWindowConstraint::ApplyModif(PDPTWData const &data, Pair const &pair, int routeIndex, int pickupPos,
//                                       int deliveryPos, bool addPair)
// {
//     std::vector<int> const &route = getSolution().getRoute(routeIndex).getRoute();

//     // Recalculate from scratch
//     std::vector<double> newArrivalTimes;
//     newArrivalTimes.resize(route.size());

//     double time = 0;
//     int prev = 0;

//     for (size_t i = 0; i < route.size(); ++i)
//     {
//         int curr = route.at(i);
//         time += data::travelCost(data, prev, curr);

//         auto const &tw = data.getLocation(curr).getTimeWindow();
//         time = std::max(time, tw.getStart());
        
//         newArrivalTimes[i] = time;

//         time += data.getLocation(curr).getServiceDuration();
//         prev = curr;

//     }

//     // update the constraint
//     arrivalTimeContainer.at(routeIndex) = std::move(newArrivalTimes);
// }

// bool TimeWindowConstraint::check(InsertPair const &op) const
// {
//     return checkInsertion(getSolution().getData(),
//                           op.getPair(),
//                           op.getRouteIndex(),
//                           op.getPickupInsertion(),
//                           op.getDeliveryInsertion());
// }

// void TimeWindowConstraint::apply(InsertPair const &op)
// {
//     ApplyModif(getSolution().getData(),
//                op.getPair(),
//                op.getRouteIndex(),
//                op.getPickupInsertion(),
//                op.getDeliveryInsertion(),
//                true);
// }

// bool TimeWindowConstraint::check(InsertRoute const &op) const
// {
//     return true;
// }

// void TimeWindowConstraint::apply(InsertRoute const &op)
// {
//     arrivalTimeContainer.emplace_back();
// }

// bool TimeWindowConstraint::check(RemovePair const &op) const
// {
//     return true;
// }

// void TimeWindowConstraint::apply(RemovePair const &op)
// {
//     ApplyModif(getSolution().getData(),
//                op.getPair(),
//                op.getRouteIndex(),
//                op.getPickupDeletion(),
//                op.getDeliveryDeletion(),
//                false);
// }

// bool TimeWindowConstraint::check(RemoveRoute const &op) const
// {
//     return true;
// }

// void TimeWindowConstraint::apply(RemoveRoute const &op)
// {
//     arrivalTimeContainer.erase(arrivalTimeContainer.begin() + op.getRouteIndex());
// }

// void TimeWindowConstraint::print() const
// {
//     int n = arrivalTimeContainer.size();
//     std::cout << "TW without FTS:" << std::endl;

//     for (int i = 0; i < n; ++i)
//     {
//         std::cout << "#" << i << ": ";
//         std::vector<double> const &arrivalTime = arrivalTimeContainer.at(i);
//         for (double const &time: arrivalTime)
//         {
//             std::cout << time << " ";
//         }
//         std::cout << std::endl;
//     }
// }
