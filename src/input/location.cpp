#include "location.h"


Location::Location(unsigned int id, double lon, double lat, int dem, TimeWindow tw, double service, unsigned int pairId, LocType type)
    : id(id), longitude(lon), latitude(lat), demand(dem), timeWindow(tw), serviceDuration(service), pairedLocation(pairId), locType(type) {}


double Location::getLongitude() const {
    return longitude;
}

double Location::getLatitude() const {
    return latitude;
}

double Location::getServiceDuration() const {
    return serviceDuration;
}

unsigned int Location::getId() const {
    return id;
}

unsigned int Location::getPair() const {
    return pairedLocation;
}

int Location::getDemand() const {
    return demand;
}

LocType Location::getLocType() const {
    return locType;
}

TimeWindow Location::getTimeWindow() const {
    return timeWindow;
}

std::string Location::LocTypeToString(LocType type) const
{
    switch (type) {
        case LocType::DEPOT: return "Depot";
        case LocType::DELIVERY: return "Delivery";
        case LocType::PICKUP: return "Pickup";
        default: return "Unknown";
    }
}


void Location::print() const
{
    std::cout << "Location ID: " << id << ", Coordinates: (" << longitude << ", " << latitude << ")\n";
    std::cout << "Location Type : " <<  Location::LocTypeToString(locType) << ", Associated location : " << pairedLocation << "\n";
    std::cout << "Demand : " <<  demand << "\n";
    timeWindow.print();
}