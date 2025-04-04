#include "capacity_constraint.h"

#include "input/location.h"
#include "input/pdptw_data.h"

#include <unistd.h>

CapacityConstraint::CapacityConstraint(Solution const &solution) : Constraint(solution)
{
    // Init an empty maxCapacity
    int n = getSolution().getData().getSize();
    maxCapacity = std::vector<std::vector<double>>(n, std::vector<double>(n, 0));
}

std::unique_ptr<Constraint> CapacityConstraint::clone(Solution const &newOwningSolution) const
{
    std::unique_ptr<CapacityConstraint> clonePtr = std::make_unique<CapacityConstraint>(newOwningSolution);
    clonePtr->maxCapacity = maxCapacity;
    return clonePtr;
}

void CapacityConstraint::initMaxCapacity()
{
    // n = number of location (depot include)
    int n = getSolution().getData().getSize();
    maxCapacity.assign(n, std::vector<double>(n, 0));

    for (Route const &route: getSolution().getRoutes())
    {
        int m = route.getSize();// depot + route
        std::vector<int> routeIDs = route.getRoute();
        std::vector<double> cumulated = std::vector<double>(m, 0.0);

        // calculate the cumulated capacity
        // + maxCharge from the depot to another location in the route
        double maxCharge = 0;
        for (int i = 0; i < m; i++)
        {
            int locationID = routeIDs.at(i);
            double demand = getSolution().getData().getLocation(locationID).getDemand();
            cumulated.at(i) = (i == 0) ? demand : cumulated.at(i - 1) + demand;

            // Why i - 1 ?, the insertion is done before i, so we look at the state of the capacity at the previous location
            if (i != 0)
            {
                maxCharge = std::max(maxCharge, cumulated.at(i - 1));
            }
            maxCapacity.at(0).at(locationID) = maxCharge;
        }

        // charge_max between i and j
        for (int i = 0; i < m; i++)
        {
            maxCharge = 0;
            int firstLocationID = routeIDs.at(i);
            if (i != 0)
            {
                maxCharge = cumulated.at(i - 1);
            }
            for (int j = i; j < m; j++)
            {
                int secondLocationID = routeIDs.at(j);
                if (j != 0)
                {
                    maxCharge = std::max(maxCharge, cumulated.at(j - 1));
                }
                maxCapacity.at(firstLocationID).at(secondLocationID) = maxCharge;
            }
        }
    }
}

void CapacityConstraint::updateMaxCapacity(Route const &route)
{
    std::vector<int> const &routeIDs = route.getRoute();
    PDPTWData const &data = getSolution().getData();
    int m = routeIDs.size();

    // Recalculate cumulated
    std::vector<double> cumulated(m, 0.0);

    double maxCharge = 0;
    for (int i = 0; i < m; ++i)
    {
        int locationID = routeIDs.at(i);
        double demand = data.getLocation(locationID).getDemand();
        cumulated.at(i) = (i == 0) ? demand : cumulated.at(i - 1) + demand;

        // Why i - 1 ?, the insertion is done before i, so we look at the state of the capacity at the previous location
        if (i != 0)
        {
            maxCharge = std::max(maxCharge, cumulated.at(i - 1));
        }
        maxCapacity.at(0).at(locationID) = maxCharge;
    }
    // Update maxCapacity
    for (int i = 0; i < m; ++i)
    {
        maxCharge = (i == 0) ? 0.0 : cumulated.at(i - 1);
        int firstLocationID = routeIDs.at(i);

        auto & maxCapacityRow = maxCapacity.at(firstLocationID);
        for (int j = i; j < m; ++j)
        {
            int secondLocationID = routeIDs.at(j);
            if (j != 0)
            {
                maxCharge = std::max(maxCharge, cumulated.at(j - 1));
            }
            maxCapacityRow.at(secondLocationID) = maxCharge;
        }
    }
}

bool CapacityConstraint::checkModif(Pair const &pair, int routeIndex, int PickupPosition, int DeliveryPosition) const
{
    Solution const &solution = getSolution();
    std::vector<int> const &routeIDs = solution.getRoutes().at(routeIndex).getRoute();
    double vehicleCapacity = solution.getData().getCapacity();

    if (PickupPosition > DeliveryPosition)
    {
        return false;
    }

    int m = routeIDs.size();

    if (m == 0)
    {
        return true;
    }

    double demand = pair.getPickup().getDemand();

    int lastLocationID = routeIDs.at(m - 1);
    int pickupLocationID = (PickupPosition >= m) ? lastLocationID : routeIDs.at(PickupPosition);
    int deliveryLocationID = (DeliveryPosition >= m) ? lastLocationID : routeIDs.at(DeliveryPosition);

    return maxCapacity.at(pickupLocationID).at(deliveryLocationID) + demand <= vehicleCapacity;
}


void CapacityConstraint::applyModif(Pair const &pair, int routeIndex, int PickupPosition, int DeliveryPosition,
                                    bool addPair)
{
    updateMaxCapacity(getSolution().getRoute(routeIndex));
}

bool CapacityConstraint::check(InsertPair const &op) const
{
    //std::cout << " #Capa Check";
    return checkModif(op.getPair(), op.getRouteIndex(), op.getPickupInsertion(), op.getDeliveryInsertion());
}

void CapacityConstraint::apply(InsertPair const &op)
{
    //std::cout << "-> Apply Modification on Capacity \n";
    applyModif(op.getPair(), op.getRouteIndex(), op.getPickupInsertion(), op.getDeliveryInsertion(), true);
}

bool CapacityConstraint::check(InsertRoute const &op) const
{
    return true;
}

void CapacityConstraint::apply(InsertRoute const &op)
{

}

bool CapacityConstraint::check(RemovePair const &op) const
{
    // Remove a pair (always true)
    return true;
}

void CapacityConstraint::apply(RemovePair const &op)
{
    applyModif(op.getPair(), op.getRouteIndex(), op.getPickupDeletion(), op.getDeliveryDeletion(), false);
}

bool CapacityConstraint::check(RemoveRoute const &op) const
{
    return true;
}

void CapacityConstraint::apply(RemoveRoute const &op)
{

}

void CapacityConstraint::print() const
{
    std::cout << "Max Capacity Matrix : " << std::endl;
    for (auto const &maxVector: maxCapacity)
    {
        for (int const maxCapa: maxVector)
        {
            std::cout << maxCapa << "\t";
        }
        std::cout << std::endl;
    }
}
