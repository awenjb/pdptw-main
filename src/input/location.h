#pragma once

#include "time_window.h"

#include <iostream>
#include <optional>
#include <string>
#include <vector>

/**
 * Enum class representing types of locations.
 */
enum class LocType
{
    DEPOT,
    PICKUP,
    DELIVERY
};

/**
 * Represents a geographic location with associated data such as demand, service time, type, and time window.
 * Used as a base for specific location types: Depot, Pickup, Delivery.
 */
class Location
{
    int id;          // Unique identifier for the location
    double longitude;// Longitude coordinate
    double latitude; // Latitude coordinate

    /**
     * Demand at this location.
     * Positive for PICKUP, negative for DELIVERY, zero for DEPOT.
     */
    int demand;

    double serviceDuration;// Time required to service the location

    /**
     * Paired location ID.
     * For pickups/deliveries, pairs with their counterpart. For depots, paired with itself.
     */
    int pairedLocationID;

    LocType locType;      // Type of the location (DEPOT, PICKUP, DELIVERY)
    TimeWindow timeWindow;// Time window during which service can occur

public:
    Location(int id, double lon, double lat, int dem, TimeWindow tw, double service, int pairId, LocType type);

    double getLongitude() const;
    double getLatitude() const;
    double getServiceDuration() const;
    int getId() const;
    int getPair() const;
    int getDemand() const;
    LocType getLocType() const;
    TimeWindow getTimeWindow() const;

    void print() const;

    /**
     * Converts a LocType enum to its string representation.
     * @param type The location type.
     * @return std::string The string representation of the location type.
     */
    static std::string LocTypeToString(LocType type);

    /**
     * JSON deserialization friend function.
     * Allows `from_json` to access private members of Location.
     */
    friend void from_json(nlohmann::json const &json, Location &location);
};