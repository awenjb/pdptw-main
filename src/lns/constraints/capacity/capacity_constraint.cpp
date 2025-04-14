#include "capacity_constraint.h"

#include "input/location.h"
#include "input/pdptw_data.h"

#include <unistd.h>

CapacityConstraint::CapacityConstraint(Solution const &solution) : Constraint(solution), n(getSolution().getData().getSize())
{
    // Init an empty maxCapacity
    maxCapacity = std::vector<double>(n * n, 0.0);
}

std::unique_ptr<Constraint> CapacityConstraint::clone(Solution const &newOwningSolution) const
{
    std::unique_ptr<CapacityConstraint> clonePtr = std::make_unique<CapacityConstraint>(newOwningSolution);
    clonePtr->maxCapacity = maxCapacity;
    return clonePtr;
}

void CapacityConstraint::updateMaxCapacity(const Route &route)
{
    const std::vector<int> &routeIDs = route.getRoute();
    const PDPTWData &data = getSolution().getData();
    int m = routeIDs.size();

    std::vector<double> cumulated(m, 0.0);
    double currentSum = 0.0;

    // Depot to Location
    for (int i = 0; i < m; ++i)
    {
        int locationID = routeIDs.at(i);
        currentSum += data.getLocation(locationID).getDemand();
        cumulated.at(i) = currentSum;

        double maxCharge = (i == 0) ? 0.0 : cumulated.at(i - 1);
        maxCapacity.at(0 * n + locationID) = maxCharge;
    }

    // Location to Location
    for (int i = 0; i < m; ++i)
    {
        int fromID = routeIDs.at(i);
        double maxCharge = (i == 0) ? 0.0 : cumulated.at(i - 1);

        for (int j = i; j < m; ++j)
        {
            if (j > 0)
            {
                maxCharge = std::max(maxCharge, cumulated.at(j - 1));
            }

            int toID = routeIDs.at(j);
            maxCapacity.at(fromID * n + toID) = maxCharge;
        }
    }
}

bool CapacityConstraint::checkModif(Pair const &pair, int routeIndex, int PickupPosition, int DeliveryPosition) const
{
    if (PickupPosition > DeliveryPosition)
    {
        return false;
    }

    Solution const &solution = getSolution();
    std::vector<int> const &routeIDs = solution.getRoutes().at(routeIndex).getRoute();
    double vehicleCapacity = solution.getData().getCapacity();
    int m = routeIDs.size();

    if (m == 0)
    {
        return pair.getPickup().getDemand() <= vehicleCapacity;
    }

    double demand = pair.getPickup().getDemand();

    int pickupIndex = (PickupPosition >= m) ? m - 1 : PickupPosition;
    int deliveryIndex = (DeliveryPosition >= m) ? m - 1 : DeliveryPosition;

    int pickupLocationID = routeIDs.at(pickupIndex);
    int deliveryLocationID = routeIDs.at(deliveryIndex);

    return maxCapacity.at(pickupLocationID * n + deliveryLocationID) + demand <= vehicleCapacity;
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
    // No-op
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
    // No-op
}

void CapacityConstraint::print() const
{
    std::cout << "Max Capacity Matrix:" << std::endl;
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            std::cout << maxCapacity.at(i * n + j) << "\t";
        }
        std::cout << std::endl;
    }
}