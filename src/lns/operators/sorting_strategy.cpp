#include "sorting_strategy.h"

#include "input/pdptw_data.h"
#include "input/data.h"
#include "utils.h"

#include <algorithm>
#include <ranges>


double getDistanceToDepot(PDPTWData const &data, int pairID)
{
    return data::TravelTime(data, 0, pairID);
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

// Following sorting strategy are based on the pickup, TO DO, sort based on the pickup and the delivery


std::vector<int> const &sorting_strategy::Close::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    // Pair ID = Pickup ID
    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        return getDistanceToDepot(getSolution().getData(), a) < getDistanceToDepot(getSolution().getData(), b);
    });
    return bank;
}

std::vector<int> const &sorting_strategy::Far::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    // Pair ID = Pickup ID
    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        return getDistanceToDepot(getSolution().getData(), a) > getDistanceToDepot(getSolution().getData(), b);
    });
    return bank;
}

std::vector<int> const &sorting_strategy::TimeWindowWidth::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    // Pair ID = Pickup ID
    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        return getSolution().getData().getLocation(a).getTimeWindow().getWidth() < getSolution().getData().getLocation(b).getTimeWindow().getWidth();
    });
    return bank;
}

std::vector<int> const &sorting_strategy::TimeWindowStart::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    // Pair ID = Pickup ID
    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        return getSolution().getData().getLocation(a).getTimeWindow().getStart() < getSolution().getData().getLocation(b).getTimeWindow().getStart();
    });
    return bank;
}

std::vector<int> const &sorting_strategy::TimeWindowEnd::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    // Pair ID = Pickup ID
    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        return getSolution().getData().getLocation(a).getTimeWindow().getEnd() > getSolution().getData().getLocation(b).getTimeWindow().getEnd();
    });
    return bank;
}