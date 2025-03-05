#include "sorting_strategy.h"

#include "utils.h"

#include <algorithm>
#include <ranges>

std::vector<int> const &sorting_strategy::Shuffle::sortPairs() const
{
    auto &bank = getSolution().getPairBank();
    std::ranges::shuffle(bank, util::getRawRandom());
    return bank;
}