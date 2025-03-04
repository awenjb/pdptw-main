#include "utils.h"

#include "config.h"

#include <random>

namespace// anonymous namespace
{
    std::mt19937_64 randomGenerator;// NOLINT(*-msc51-cpp) => deterministic random!
    bool seedSet = false;
    std::uniform_real_distribution<> distribution(0, 1);
}// namespace

double util::getRandom()
{
    if (!seedSet) [[unlikely]]
    {
        randomGenerator.seed(RANDOM_SEED);
        seedSet = true;
    }
    return distribution(randomGenerator);
}

unsigned int util::getRandomInt(unsigned int min, unsigned int max)
{
    if (!seedSet) [[unlikely]]
    {
        randomGenerator.seed(RANDOM_SEED);
        seedSet = true;
    }
    return std::uniform_int_distribution<>(min, max)(randomGenerator);
}

std::mt19937_64 &util::getRawRandom()
{
    if (!seedSet) [[unlikely]]
    {
        randomGenerator.seed(RANDOM_SEED);
        seedSet = true;
    }
    return randomGenerator;
}
