#include "solution_checker.h"

#include "input/data.h"
#include "input/location.h"
#include "input/pdptw_data.h"
#include "lns/solution/solution.h"
#include "types.h"

#include <spdlog/spdlog.h>
#include <vector>

void checker::checkSolutionCoherence(Solution const &sol, PDPTWData const &data)
{
    std::cout << " --- Solution Coherence checker : ";

    bool errorFlag = false;

    // Vector that will store the route ID serving the location, (-1 if no routes)
    // Caution ! check is indexed from 0 to n-1 and Location ID are indexed from 1 to n
    std::vector<int> check(data.getLocations().size(), -1);

    // checking routes coherence
    int routeID = 0;
    for (Route const &route: sol.getRoutes())
    {   
        std::cout << "#";
        // skip if it is an empty route
        if (!route.getRoute().empty())
        {
            for (int LocID: route.getRoute())
            {
                if (check.at(LocID -1) != -1)
                {
                    std::cout << "#";

                    // Error the location is already attributed (doublon)
                    spdlog::error("Location {} has already been visited.", LocID);
                    errorFlag = true;
                }

                check.at(LocID -1) = routeID;

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

    std::cout << "#";

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
        sol.print();
        throw SolutionConstraintError("Error in the consistency of the solution.", sol);
    }

    std::cout << " : checker end ---\n";
}

void checker::checkCapacity(Solution const &sol, PDPTWData const &data)
{
    bool errorFlag = false;
    int capa = 0;
    int routeID = 0;

    for (Route const &route: sol.getRoutes())
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
    TimeInteger reachTime = 0;
    int routeID = 0;
    int locID = 0;

    for (Route const &route: sol.getRoutes())
    {
        // travel to the first location
        reachTime = data.getDepot().getTimeWindow().getStart() + data::TravelTime(data, 0, route.getRoute().at(0));

        for (int i = 0; i < route.getRoute().size() - 1; i++)
        {
            locID = route.getRoute().at(i);
            // check TimeWindow
            if (!(data.getLocation(locID).getTimeWindow().isValid(reachTime)))
            {
                // Error, reach time not valid for the location time window
                spdlog::error("Reach time not valid for the location {} time window in route {}.", locID, routeID);
                errorFlag = true;
            }

            TimeInteger travelTime = data::TravelTime(data, locID, route.getRoute().at(i + 1));
            TimeInteger serviceTime = data.getLocation(locID).getServiceDuration();
            TimeInteger startTW = data.getLocation(locID).getTimeWindow().getStart();

            reachTime = std::max(reachTime, startTW) + serviceTime + travelTime;
        }

        // check last timeWindow
        locID = route.getRoute().back();
        if (!(data.getLocation(locID).getTimeWindow().isValid(reachTime)))
        {
            // Error, reach time not valid for the location time window
            spdlog::error("Reach time not valid for the location {} time window in route {}.", locID, routeID);
            errorFlag = true;
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
