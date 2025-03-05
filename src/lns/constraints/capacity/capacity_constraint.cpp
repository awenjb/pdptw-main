#include "capacity_constraint.h"


CapacityConstraint::CapacityConstraint(Solution const &solution) : Constraint(solution)
{
    // Init an empty vector of reach time for all routes
    //std::vector<CapacityVector> routeCapacities(getSolution().getRoutes().size(), CapacityVector());
    routeCapacities.resize(getSolution().getRoutes().size());
}

std::unique_ptr<Constraint> CapacityConstraint::clone(Solution const &newOwningSolution) const {
    std::unique_ptr<CapacityConstraint> clonePtr = std::make_unique<CapacityConstraint>(newOwningSolution);
    clonePtr->routeCapacities = routeCapacities;
    return clonePtr;
}


void CapacityConstraint::initCapacities()
{
    routeCapacities = std::vector<CapacityVector>();
    for (const Route& route : getSolution().getRoutes()) 
    {
        CapacityVector capacities;
        int currentCapacity = 0;
        
        for (int locationID : route.getRoute()) 
        {
            currentCapacity += getSolution().getData().getLocation(locationID).getDemand();
            capacities.push_back(currentCapacity);
        }
        
        routeCapacities.push_back(capacities);
    }
}


std::vector<int> const & CapacityConstraint::getRouteCapacities(int routeIndex) const
{
    return routeCapacities[routeIndex];
}

// check for every location between the pickupPosition and the deliveryPosition
// not ideal
bool CapacityConstraint::checkModif(Pair const &pair, int routeIndex, int PickupPosition, int DeliveryPosition) const
{
    std::cout << "\n";
    int max_capacity = getSolution().getData().getCapacity();

    // guardrail
    if (pair.getPickup().getDemand() > max_capacity)
    {
        return false;
    }

    // check if the disponible capacity is enough are not
    for (int i = PickupPosition; i <= DeliveryPosition; ++i) {
        
        if ((i!=0) && (getRouteCapacities(routeIndex).at(i-1) + pair.getPickup().getDemand() >= max_capacity))
        {
            return false;
        }
    } 
    return true;
}

// update the route and the weight
// not ideal
void CapacityConstraint::applyModif(Pair const &pair, int routeIndex, int PickupPosition,  int DeliveryPosition, bool addPair)
{
    std::cout << "ApplyModif (capa constraint) \n";
    if (addPair)
    {   
        // Insert new values
        routeCapacities[routeIndex].insert(routeCapacities[routeIndex].begin()+DeliveryPosition, 0);
        if (DeliveryPosition != 0)
        {
            routeCapacities[routeIndex][DeliveryPosition] += routeCapacities[routeIndex][DeliveryPosition-1];
        }
        routeCapacities[routeIndex].insert(routeCapacities[routeIndex].begin()+PickupPosition, pair.getPickup().getDemand());
        if (PickupPosition != 0)
        {
            routeCapacities[routeIndex][PickupPosition] += routeCapacities[routeIndex][PickupPosition-1];
        }

        // Update value
        for (int i = PickupPosition + 1; i < DeliveryPosition + 1; ++i) 
        {
            routeCapacities[routeIndex][i] += pair.getPickup().getDemand();
        } 
    }
    else 
    {
        for (int i = PickupPosition + 1; i < DeliveryPosition; ++i) 
        {
            routeCapacities[routeIndex][i] += pair.getDelivery().getDemand();
        }

        // remove pair
        routeCapacities[routeIndex].erase(routeCapacities[routeIndex].begin() + DeliveryPosition);
        routeCapacities[routeIndex].erase(routeCapacities[routeIndex].begin() + PickupPosition);
    }    
}


bool CapacityConstraint::check(InsertPair const &op) const
{
    std::cout << "capa constraint \n";
    return checkModif(op.getPair(), op.getRouteIndex(), op.getPickupInsertion(), op.getDeliveryInsertion());
}
void CapacityConstraint::apply(InsertPair const &op)
{
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
    for (const auto& capaVector : routeCapacities)
    {
        std::cout << "#" << i << " : ";
        for (const int capa : capaVector)
        {
            std::cout << capa << ", ";
        }
        std::cout << "\n ";
        i++;
    }
    std::cout << "\n";
}

