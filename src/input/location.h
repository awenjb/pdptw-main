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
    /*
    *   The location ID 
    */
    int id;
    double longitude;
    double latitude;
    /*
    *   Represent the capacity used by that location.
    *   The location demand must be positive if it is a PICKUP, negative if it is a DELIVERY and 0 for a DEPOT.
    */
    int demand;
    double serviceDuration;
    /*
    *   The location ID associated to form a pickup and delivery pair.
    *   (the same ID if the location is a depot)
    */
    int pairedLocationID;
    /*
    *   The type of the location (PICKUP, DELIVERY, DEPOT).
    */
    LocType locType;
    TimeWindow timeWindow;

public:

    Location(int id, double lon, double lat, int dem, TimeWindow tw, double service, int pairId,  LocType type);

    double getLongitude() const;
    double getLatitude() const;
    double getServiceDuration() const;
    int getId() const;
    int getPair() const;
    int getDemand() const;
    LocType getLocType() const;
    TimeWindow getTimeWindow() const;

    void print() const;

    static std::string LocTypeToString(LocType type);
    
    // Json parsing
    friend void from_json(nlohmann::json const &json, Location &location);
};