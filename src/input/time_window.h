#pragma once

#include "types.h"

#include <nlohmann/json_fwd.hpp>

/**
 * Represents a time window [start, end] with basic utility methods.
 * A TimeWindow is defined by two time points: a start and an end.
 * It provides methods to access boundaries and check inclusion.
 */
class TimeWindow
{
    TimeInteger start, end;// Start and end of the time window (inclusive)

public:
    TimeWindow(TimeInteger s, TimeInteger e);
    TimeInteger getStart() const;
    TimeInteger getEnd() const;
    TimeInteger getWidth() const;

    /**
     * Checks whether a time is strictly within the window [start, end].
     * @param t Time to check
     * @return true if t is between start and end (inclusive), false otherwise
     */
    bool isIn(TimeInteger t) const;

    /**
     * Checks whether a time is before or within the window.
     * @param t Time to check
     * @return true if t <= end, false otherwise
     */
    bool isValid(TimeInteger t) const;

    void print() const;

    friend void from_json(nlohmann::json const &json, TimeWindow &tw);
};