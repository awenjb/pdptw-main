#pragma once

#include<string>
#include <optional>
#include <iostream>
#include <vector>

#include "time_window.h"

/**
 * A type of location
 */
enum class LocType
{
    DEPOT,
    PICKUP,
    DELIVERY
};

/**
 * Represent a location in space combined with a type and a time window
 * @see Depot, Pickup, Delivery
 */
class Location
{
    unsigned int id;
    double longitude;
    double latitude;
    int demand;
    double serviceDuration;
    unsigned int pairedLocation;
    LocType locType;
    TimeWindow timeWindow;

public:

    Location(unsigned int id, double lon, double lat, int dem, TimeWindow tw, double service, unsigned int pairId,  LocType type);

    double getLongitude() const;
    double getLatitude() const;
    double getServiceDuration() const;
    unsigned getId() const;
    unsigned int getPair() const;
    int getDemand() const;
    LocType getLocType() const;
    TimeWindow getTimeWindow() const;

    void print() const;
    std::string LocTypeToString(LocType type) const;

    // Json parsing
    friend void from_json(nlohmann::json const &json, Location &location);
};