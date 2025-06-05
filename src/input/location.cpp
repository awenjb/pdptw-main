#include "location.h"

Location::Location(int id, double lon, double lat, int dem, TimeWindow tw, double service, int pairId, LocType type)
    : id(id), longitude(lon), latitude(lat), demand(dem), timeWindow(tw), serviceDuration(service),
      pairedLocationID(pairId), locType(type)
{}

double Location::getLongitude() const
{
    return longitude;
}

double Location::getLatitude() const
{
    return latitude;
}

double Location::getServiceDuration() const
{
    return serviceDuration;
}

int Location::getId() const
{
    return id;
}

int Location::getPair() const
{
    return pairedLocationID;
}

int Location::getDemand() const
{
    return demand;
}

LocType Location::getLocType() const
{
    return locType;
}

TimeWindow Location::getTimeWindow() const
{
    return timeWindow;
}

// Conversion enum -> string
std::string Location::LocTypeToString(LocType type)
{
    switch (type)
    {
        case LocType::DEPOT:
            return "Depot";
        case LocType::DELIVERY:
            return "Delivery";
        case LocType::PICKUP:
            return "Pickup";
        default:
            return "Unknown";
    }
}

// Display
void Location::print() const
{
    std::cout << "Location ID: " << id << ", Coordinates: (" << longitude << ", " << latitude << ")\n";
    std::cout << "Location Type : " << Location::LocTypeToString(locType)
              << ", Associated location : " << pairedLocationID << "\n";
    std::cout << "Demand : " << demand << ", Service Duration :" << serviceDuration << "\n";
    timeWindow.print();
}