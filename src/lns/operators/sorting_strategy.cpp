#include "sorting_strategy.h"

#include "input/data.h"
#include "input/pdptw_data.h"
#include "utils.h"

#include <algorithm>
#include <ranges>

double getDistanceToDepot(PDPTWData const &data, int pairID)
{
    return data::travelCost(data, 0, pairID);
}

std::vector<int> const &sorting_strategy::LastInFirstOut::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    return bank;
}

std::vector<int> const &sorting_strategy::FirstInFirstOut::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    std::reverse(bank.begin(), bank.end());
    return bank;
}

std::vector<int> const &sorting_strategy::Shuffle::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    std::ranges::shuffle(bank, util::getRawRandom());
    return bank;
}

std::vector<int> const &sorting_strategy::Demand::sortPairs() const
{
    auto &bank = getSolution().getPairBank();

    // Pair ID = Pickup ID
    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        return getSolution().getData().getLocation(a).getDemand() > getSolution().getData().getLocation(b).getDemand();
    });
    return bank;
}

std::vector<int> const &sorting_strategy::Close::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    // Pair ID = Pickup ID
    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        return (getDistanceToDepot(getSolution().getData(), a) +
                getDistanceToDepot(getSolution().getData(), getSolution().getData().getLocation(a).getPair())) /
                       2 <
               (getDistanceToDepot(getSolution().getData(), b) +
                getDistanceToDepot(getSolution().getData(), getSolution().getData().getLocation(b).getPair())) /
                       2;
    });
    return bank;
}

std::vector<int> const &sorting_strategy::Far::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    // Pair ID = Pickup ID
    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        return (getDistanceToDepot(getSolution().getData(), a) +
                getDistanceToDepot(getSolution().getData(), getSolution().getData().getLocation(a).getPair())) /
                       2 >
               (getDistanceToDepot(getSolution().getData(), b) +
                getDistanceToDepot(getSolution().getData(), getSolution().getData().getLocation(b).getPair())) /
                       2;
    });
    return bank;
}

std::vector<int> const &sorting_strategy::TimeWindowWidth::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    // Pair ID = Pickup ID
    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        const Location &locA = getSolution().getData().getLocation(a);
        const Location &locB = getSolution().getData().getLocation(b);
        return locA.getTimeWindow().getWidth() +
                       getSolution().getData().getLocation(locA.getPair()).getTimeWindow().getWidth() / 2 <
               locB.getTimeWindow().getWidth() +
                       getSolution().getData().getLocation(locB.getPair()).getTimeWindow().getWidth() / 2;
    });
    return bank;
}

std::vector<int> const &sorting_strategy::TimeWindowStart::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    // Pair ID = Pickup ID
    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        return getSolution().getData().getLocation(a).getTimeWindow().getStart() <
               getSolution().getData().getLocation(b).getTimeWindow().getStart();
    });
    return bank;
}

std::vector<int> const &sorting_strategy::TimeWindowEnd::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    // Pair ID = Pickup ID
    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        const Location &locA = getSolution().getData().getLocation(a);
        const Location &locB = getSolution().getData().getLocation(b);
        return getSolution().getData().getLocation(locA.getPair()).getTimeWindow().getEnd() >
               getSolution().getData().getLocation(locB.getPair()).getTimeWindow().getEnd();
    });
    return bank;
}