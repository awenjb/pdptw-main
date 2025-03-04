#pragma once

#include <algorithm>
#include <functional>
#include <memory>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace util
{
    /**
     * Get a random number
     * @return a number between 0 (inclusive) and 1 (exclusive)
     */
    double getRandom();
    /**
     * @return a random integer number between min (included) and max (included)
     */
    unsigned int getRandomInt(unsigned int min, unsigned int max);


}// namespace util
