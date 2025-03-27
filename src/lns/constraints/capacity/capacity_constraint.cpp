#include "capacity_constraint.h"

CapacityConstraint::CapacityConstraint(Solution const &solution) : Constraint(solution)
{
    // Init an empty vector of reach time for all routes
    routeCapacities.resize(getSolution().getRoutes().size());
}

std::unique_ptr<Constraint> CapacityConstraint::clone(Solution const &newOwningSolution) const
{
    std::unique_ptr<CapacityConstraint> clonePtr = std::make_unique<CapacityConstraint>(newOwningSolution);
    clonePtr->routeCapacities = routeCapacities;
    return clonePtr;
}

void CapacityConstraint::initRouteCapacities()
{
    routeCapacities.clear();
    for (Route const &route: getSolution().getRoutes())
    {
        CapacityVector capacities;
        int currentCapacity = 0;

        for (int locationID: route.getRoute())
        {
            currentCapacity += getSolution().getData().getLocation(locationID).getDemand();
            capacities.push_back(currentCapacity);
        }

        routeCapacities.push_back(capacities);
    }
}


std::vector<int> const &CapacityConstraint::getRouteCapacities(int routeIndex) const
{
    return routeCapacities.at(routeIndex);
}

/*
 *  Check if an insertion is feasible by computing new capacity values between the PickupPosition and DeliveryPosition.
 *  Assume the constraint is valid beforehand.
 *  O(n), n the size of the route 
 */
bool CapacityConstraint::checkModif(Pair const &pair, int routeIndex, int PickupPosition, int DeliveryPosition) const
{
    int max_capacity = getSolution().getData().getCapacity();
    int demand = pair.getPickup().getDemand();
    CapacityVector const &routeCapacity = getRouteCapacities(routeIndex);

    // if PickupPosition != DeliveryPosition, check from pickup-1 to delivery-1
    // if PickupPosition is 0, don't check -1 ...
    for (int i = std::max(0, PickupPosition - 1); i < DeliveryPosition; i++)
    {
        if (routeCapacity.at(i) + demand > max_capacity)
        {
            return false;
        }
    }
    return true;
}

// update the route and the weight
void CapacityConstraint::applyModif(Pair const &pair, int routeIndex, int PickupPosition, int DeliveryPosition,
                                    bool addPair)
{
    if (addPair)
    {
        // Insert new values
        routeCapacities.at(routeIndex).insert(routeCapacities.at(routeIndex).begin() + DeliveryPosition, 0);
        if (DeliveryPosition != 0)
        {
            routeCapacities.at(routeIndex).at(DeliveryPosition) +=
                    routeCapacities.at(routeIndex).at(DeliveryPosition - 1);
        }
        routeCapacities.at(routeIndex)
                .insert(routeCapacities.at(routeIndex).begin() + PickupPosition, pair.getPickup().getDemand());
        if (PickupPosition != 0)
        {
            routeCapacities.at(routeIndex).at(PickupPosition) += routeCapacities.at(routeIndex).at(PickupPosition - 1);
        }

        // Update value
        for (int i = PickupPosition + 1; i < DeliveryPosition + 1; ++i)
        {
            routeCapacities.at(routeIndex).at(i) += pair.getPickup().getDemand();
        }
    }
    else
    {
        for (int i = PickupPosition + 1; i < DeliveryPosition; ++i)
        {
            routeCapacities.at(routeIndex).at(i) += pair.getDelivery().getDemand();
        }

        // remove pair
        routeCapacities.at(routeIndex).erase(routeCapacities.at(routeIndex).begin() + DeliveryPosition);
        routeCapacities.at(routeIndex).erase(routeCapacities.at(routeIndex).begin() + PickupPosition);
    }
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
    // add new empty route
    routeCapacities.emplace_back();
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
    // add new empty route
    routeCapacities.erase(routeCapacities.begin() + op.getRouteIndex());
}

void CapacityConstraint::print() const
{
    std::cout << "Used capacity : \n";
    int i = 0;
    for (auto const &capaVector: routeCapacities)
    {
        std::cout << "#" << i << " : ";
        for (int const capa: capaVector)
        {
            std::cout << capa << ", ";
        }
        std::cout << "\n";
        i++;
    }
}
