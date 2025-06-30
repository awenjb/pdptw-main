#include "solution_checker.h"

#include "config.h"
#include "input/data.h"
#include "input/load_dependent.h"
#include "input/location.h"
#include "input/pdptw_data.h"
#include "lns/operators/destruction/clean_empty_route.h"
#include "lns/solution/solution.h"
#include "types.h"

#include <spdlog/spdlog.h>
#include <vector>

void checker::checkSolutionCoherence(Solution const &sol, PDPTWData const &data)
{
    bool errorFlag = false;

    // Vector that will store the route ID serving the location, (-1 if no routes)
    // Caution ! check is indexed from 0 to n-1 and Location ID are indexed from 1 to n
    std::vector<int> check(data.getLocations().size(), -1);

    // checking routes coherence
    int routeID = 0;
    for (Route const &route: sol.getRoutes())
    {
        // skip if it is an empty route
        if (!route.getRoute().empty())
        {
            for (int LocID: route.getRoute())
            {
                if (check.at(LocID - 1) != -1)
                {
                    // Error the location is already attributed (doublon)
                    spdlog::error("Location {} has already been visited.", LocID);
                    errorFlag = true;
                }

                check.at(LocID - 1) = routeID;

                // if the location is a delivery, check if the pickup location has already been visited in the same route
                if ((data.getLocation(LocID).getLocType() == LocType::DELIVERY) &&
                    (check.at(data.getLocation(LocID).getPair() - 1) != routeID))
                {
                    // Error Pickup and Delivery are not in the same route (wrong route)
                    // OR Delivery before Pickup (wrong order)
                    spdlog::error("Pair associated to {} is not consistent (route or order problem).", LocID);
                    errorFlag = true;
                }
            }
        }
        ++routeID;
    }


    // checking PairBank coherence (given the routes)
    for (int pairID: sol.getBank())
    {
        // check goes from 0 et n-1
        if ((check.at(pairID - 1) != -1) || (check.at(data.getLocation(pairID).getPair() - 1) != -1))
        {
            // Error Pair in the bank but one location of the pair seems to be visited by a route (doublon)
            spdlog::error("Pair associated to {} is in a route and in the bank at the same time.", pairID);
            errorFlag = true;
        }
    }

    if (errorFlag)
    {
        throw SolutionConstraintError("Error in the consistency of the solution.", sol);
    }
}

void checker::checkCapacity(Solution const &sol, PDPTWData const &data)
{
    bool errorFlag = false;
    int capa = 0;
    int routeID = 0;

    for (Route const &route: sol.getRoutes())
    {
        if (!route.getRoute().empty())
        {
            for (int id: route.getRoute())
            {
                capa += data.getLocation(id).getDemand();
                if (capa > data.getCapacity())
                {
                    // Error, max capacity is exceeded
                    spdlog::error("Maximum Capacity is exceeded at {} in the route {}.", id, routeID);
                    errorFlag = true;
                }
            }
            if (capa != 0)
            {
                // Error, all the capacity is supposed to be free at the end of a route
                spdlog::error("Some capacity still used at the end of the route {}.", routeID);
                errorFlag = true;
            }
        }
        ++routeID;
    }

    if (errorFlag)
    {
        throw SolutionConstraintError("Error in the capacity constraint of the solution.", sol);
    }
}

void checker::checkTimeWindows(Solution const &sol, PDPTWData const &data)
{
    bool errorFlag = false;
    TimeInteger time = 0;
    double travelTime = 0;
    int routeID = 0;
    double slope = 0;
    double load = 0;

    for (Route const &route: sol.getRoutes())
    {
        if (!route.getRoute().empty())
        {
            int prev = 0;
            int curr = 0;
            load = 0;
            time = data.getDepot().getTimeWindow().getStart();

            for (size_t i = 0; i < route.getRoute().size(); ++i)
            {
                curr = route.getRoute().at(i);

                travelTime = data::travelCost(data, prev, curr);

                if (ELEVATION)
                {
                    load += data.getLocation(curr).getDemand();
                    travelTime = ltt::getTravelTimeLTT(data, load, prev, curr);
                    // travelTime = ltt::fontaineCalculation(data.getSegmentDistanceMatrix().at(prev).at(curr),
                    //                                       data.getSegmentSlopeMatrix().at(prev).at(curr),
                    //                                       load);
                }

                time += travelTime;

                if (!data.getLocation(curr).getTimeWindow().isValid(time))
                {
                    spdlog::error("Arrival time not valid for the location {} time window in route {}.", curr, routeID);
                    errorFlag = true;
                }

                time = std::max(time, data.getLocation(curr).getTimeWindow().getStart());
                time += data.getLocation(curr).getServiceDuration();

                prev = curr;
            }

            // check return to depot

            travelTime = data::travelCost(data, prev, 0);
            if (ELEVATION)
            {
                load += data.getLocation(prev).getDemand();
                travelTime = ltt::getTravelTimeLTT(data, load, prev, 0);
                // travelTime = ltt::fontaineCalculation(data.getSegmentDistanceMatrix().at(prev).at(0),
                //                                       data.getSegmentSlopeMatrix().at(prev).at(0),
                //                                       load);
            }

            if (!data.getLocation(0).getTimeWindow().isValid(time))
            {
                spdlog::error("Arrival time not valid for the location {} time window in route {}.", curr, routeID);
                errorFlag = true;
            }
        }
        ++routeID;
    }

    if (errorFlag)
    {
        throw SolutionConstraintError("Error in the time windows constraint of the solution.", sol);
    }
}

void checker::checkAll(Solution const &sol, PDPTWData const &data, bool checkRequests)
{
    // TO DO, check in the solution if all locations are visited ! (in checkSolutionCoherence ?)
    if (checkRequests)
    {
        if (!sol.getBank().empty())
        {
            // Error, all locations are supposed to be visited, the pairBank is not empty
            spdlog::error("All locations are supposed to be visited, the pairBank is not empty.");
        }
    }

    checker::checkSolutionCoherence(sol, data);
    checker::checkCapacity(sol, data);
    checker::checkTimeWindows(sol, data);
}

// No logging yet

checker::SolutionInternalError::SolutionInternalError(std::string reason, Solution const &sol)
    : std::logic_error(reason /*+ " More details in logs"*/)
{
    // logging::errorDumpLogger().error("{}\n{}", reason, sol);
    // sol.exportDotFile();
}

checker::SolutionConstraintError::SolutionConstraintError(std::string reason, Solution const &sol)
    : std::logic_error(reason /*+ " More details in logs"*/)
{
    // logging::errorDumpLogger().error("{}\n{}", reason, sol);
}
