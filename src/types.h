#pragma once

#include <iostream>
#include <limits>
#include <tuple>
#include <vector>

/**
 *   Represent a pair (pickup/delivery) position in a solution.
 *   (Route Index, Pickup Position, Delivery Position).
 *   Used in insertion/deletion modifications.
 */
using Index = std::tuple<int, int, int>;

/**
 *  A point in time or a duration.
 */
using TimeInteger = double;
TimeInteger constexpr UNDEF_TIMESTAMP = std::numeric_limits<TimeInteger>::max();


using Matrix = std::vector<std::vector<double>>; 

