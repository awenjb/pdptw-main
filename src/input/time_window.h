#pragma once

#include <iostream>
#include <limits>
#include <nlohmann/json_fwd.hpp>

/**
 * A point in time or a duration.
 */
using TimeInteger = double;
TimeInteger constexpr UNDEF_TIMESTAMP = std::numeric_limits<TimeInteger>::max();

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

    /**
     * return the time to wait from t to the start of this time window.
     * @return 0 if t is after start, or start - t
     */
    //TimeInteger waitingTimeBefore(TimeInteger t) const;

    void print() const;
    friend void from_json(nlohmann::json const &json, TimeWindow &tw);
};