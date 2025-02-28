#pragma once

#include <nlohmann/json_fwd.hpp>

#include "types.h"

/**
 * Represents a time window [start, end] with some basic utilities.
 */
class TimeWindow
{
    TimeInteger start, end;

public:
    TimeWindow(TimeInteger s, TimeInteger e);
    TimeInteger getStart() const;
    TimeInteger getEnd() const;
    TimeInteger getWidth() const;

    //Checks whether the time t is inside the time window
    bool isIn(TimeInteger t) const;

    //Checks whether the time t is inside the time window OR before
    bool isValid(TimeInteger t) const;

    void print() const;

    friend void from_json(nlohmann::json const &json, TimeWindow &tw);
};