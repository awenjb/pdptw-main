#include "utils.h"

#include "config.h"

#include <iostream>
#include <random>
#include <chrono>

namespace// anonymous namespace
{
    std::mt19937_64 randomGenerator;// NOLINT(*-msc51-cpp) => deterministic random!
    bool seedSet = false;
    std::uniform_real_distribution<> distribution(0, 1);
}// namespace


/*
 *  If SEED_SET = true, set the seed to RANDOM_SEED, otherwise set the seed to a random value
 */
void checkSeed()
{
    if (!seedSet) [[unlikely]]
    {
        if (SEED_SET)
        {
            randomGenerator.seed(RANDOM_SEED);
        }
        else
        {
            std::random_device rd;
            randomGenerator.seed(rd());
        }
        seedSet = true;
    }
}

double util::getRandom()
{
    checkSeed();
    return distribution(randomGenerator);
}

unsigned int util::getRandomInt(unsigned int min, unsigned int max)
{
    checkSeed();
    return std::uniform_int_distribution<>(min, max)(randomGenerator);
}

std::mt19937_64 &util::getRawRandom()
{
    checkSeed();
    return randomGenerator;
}
