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
    return getSolution().getPairBank();
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
    auto const &data = getSolution().getData();

    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        return data.getLocation(a).getDemand() > data.getLocation(b).getDemand();
    });

    return bank;
}

std::vector<int> const &sorting_strategy::Close::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    auto const &data = getSolution().getData();

    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        int pairA = data.getLocation(a).getPair();
        int pairB = data.getLocation(b).getPair();

        double avgDistA = (getDistanceToDepot(data, a) + getDistanceToDepot(data, pairA)) / 2.0;
        double avgDistB = (getDistanceToDepot(data, b) + getDistanceToDepot(data, pairB)) / 2.0;

        return avgDistA < avgDistB;
    });

    return bank;
}

std::vector<int> const &sorting_strategy::Far::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    auto const &data = getSolution().getData();

    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        int pairA = data.getLocation(a).getPair();
        int pairB = data.getLocation(b).getPair();

        double avgDistA = (getDistanceToDepot(data, a) + getDistanceToDepot(data, pairA)) / 2.0;
        double avgDistB = (getDistanceToDepot(data, b) + getDistanceToDepot(data, pairB)) / 2.0;

        return avgDistA > avgDistB;
    });

    return bank;
}

std::vector<int> const &sorting_strategy::TimeWindowWidth::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    auto const &data = getSolution().getData();

    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        const auto &locA = data.getLocation(a);
        const auto &locB = data.getLocation(b);
        int pairA = locA.getPair();
        int pairB = locB.getPair();

        double widthA = (locA.getTimeWindow().getWidth() + data.getLocation(pairA).getTimeWindow().getWidth()) / 2.0;
        double widthB = (locB.getTimeWindow().getWidth() + data.getLocation(pairB).getTimeWindow().getWidth()) / 2.0;

        return widthA < widthB;
    });

    return bank;
}

std::vector<int> const &sorting_strategy::TimeWindowStart::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    auto const &data = getSolution().getData();

    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        return data.getLocation(a).getTimeWindow().getStart() < data.getLocation(b).getTimeWindow().getStart();
    });

    return bank;
}

std::vector<int> const &sorting_strategy::TimeWindowEnd::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    auto const &data = getSolution().getData();

    std::sort(bank.begin(), bank.end(), [&](int a, int b) {
        int pairA = data.getLocation(a).getPair();
        int pairB = data.getLocation(b).getPair();

        return data.getLocation(pairA).getTimeWindow().getEnd() > data.getLocation(pairB).getTimeWindow().getEnd();
    });

    return bank;
}