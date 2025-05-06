#include "time_window_constraint.h"

#include "input/data.h"
#include "input/pdptw_data.h"
#include "input/time_window.h"
#include "lns/constraints/time_window/forward_time_slack.h"
#include "lns/modification/pair/insert_pair.h"
#include "lns/modification/pair/remove_pair.h"
#include "lns/modification/route/remove_route.h"
#include "lns/solution/solution.h"

TimeWindowConstraint::TimeWindowConstraint(Solution const &solution) : Constraint(solution)
{
    FTSContainer.clear();
    for (Route const &route: solution.getRoutes())
    {
        FTSContainer.emplace_back();
    }
}

TimeWindowConstraint::~TimeWindowConstraint()
{
    FTSContainer.clear();
}

std::unique_ptr<Constraint> TimeWindowConstraint::clone(Solution const &newOwningSolution) const
{
    std::unique_ptr<TimeWindowConstraint> clonePtr = std::make_unique<TimeWindowConstraint>(newOwningSolution);
    clonePtr->FTSContainer = FTSContainer;
    return clonePtr;
}

void TimeWindowConstraint::initFTS()
{
    FTSContainer = std::vector<ForwardTimeSlack>();
    //reserve
    int i = 0;
    for (Route const &route: getSolution().getRoutes())
    {
        FTSContainer.emplace_back();
        FTSContainer.at(i).initFTS(getSolution().getData(), route);
        ++i;
    }
}

bool TimeWindowConstraint::checkInsertion(PDPTWData const &data, Pair const &pair, int routeIndex, int pickupPos,
                                          int deliveryPos) const
{
    return FTSContainer.at(routeIndex)
            .isPickupDeliveryInsertionValid(data,
                                            getSolution().getRoute(routeIndex),
                                            pair.getPickup().getId(),
                                            pair.getDelivery().getId(),
                                            pickupPos,
                                            deliveryPos);
}


void TimeWindowConstraint::ApplyModif(PDPTWData const &data, Pair const &pair, int routeIndex, int pickupPos,
                                      int deliveryPos, bool addPair)
{
    if (addPair)
    {
        FTSContainer.at(routeIndex)
                .updateFTSAfterInsertion(data, getSolution().getRoute(routeIndex), pickupPos, deliveryPos);
    }
    else
    {
        FTSContainer.at(routeIndex)
                .updateFTSAfterDeletion(data, getSolution().getRoute(routeIndex), pickupPos, deliveryPos);
    }

}

bool TimeWindowConstraint::check(InsertPair const &op) const
{
    return checkInsertion(getSolution().getData(),
                          op.getPair(),
                          op.getRouteIndex(),
                          op.getPickupInsertion(),
                          op.getDeliveryInsertion());
}

void TimeWindowConstraint::apply(InsertPair const &op)
{
    ApplyModif(getSolution().getData(),
               op.getPair(),
               op.getRouteIndex(),
               op.getPickupInsertion(),
               op.getDeliveryInsertion(),
               true);
}

bool TimeWindowConstraint::check(InsertRoute const &op) const
{
    return true;
}

void TimeWindowConstraint::apply(InsertRoute const &op)
{
    FTSContainer.emplace_back();
}

bool TimeWindowConstraint::check(RemovePair const &op) const
{
    return true;
}

void TimeWindowConstraint::apply(RemovePair const &op)
{
    ApplyModif(getSolution().getData(),
               op.getPair(),
               op.getRouteIndex(),
               op.getPickupDeletion(),
               op.getDeliveryDeletion(),
               false);
}

bool TimeWindowConstraint::check(RemoveRoute const &op) const
{
    return true;
}

void TimeWindowConstraint::apply(RemoveRoute const &op)
{
    FTSContainer.erase(FTSContainer.begin() + op.getRouteIndex());
}

void TimeWindowConstraint::print() const
{
    std::cout << "Time Window : Earliest / Latest / FTS" << std::endl;
    int i = 0;
    for (ForwardTimeSlack FTS: FTSContainer)
    {
        std::cout << "#" << i << std::endl;
        FTS.print();
        std::cout << std::endl;
        i++;
    }
}
