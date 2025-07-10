#pragma once

#include "location.h"
#include "pair.h"
#include "types.h"

#include <nlohmann/json_fwd.hpp>
#include <vector>

using json = nlohmann::json;

/**
 * Exception thrown when input data validation fails.
 */
class InputJsonException : public std::exception
{
    std::string reason;

public:
    explicit InputJsonException(std::string_view reason);
    char const *what() const noexcept override;
};

/**
 * Represents the data structure for a PDPTW (Pickup and Delivery Problem with Time Windows) instance.
 */
class PDPTWData
{
    std::string dataName;           // Name of the problem instance
    int size;                       // Total number of locations (including depot)
    int capacity;                   // Maximum vehicle capacity
    Location depot;                 // Depot location
    std::vector<Location> locations;// List of customer locations (pickup and delivery)

    // List of pickup-delivery pairs; ?? consider unordered_map<int, Pair> for faster access if needed ??
    std::vector<Pair> pairs;

    Matrix costMatrix;// Cost matrix (distance)
    Matrix timeMatrix;// Time travel matrix

    // For each location, a sorted list of other location IDs by proximity (excluding depot)
    std::vector<std::vector<int>> closestLocations;

    // Segment-level data between all pairs of locations
    std::vector<std::vector<std::vector<double>>> segmentSlopeMatrix;
    std::vector<std::vector<std::vector<double>>> segmentDistanceMatrix;

    // Precalculated penalty values (for elevation)
    std::vector<std::vector<std::tuple<double, double>>> preCalculation;

    // Initializes the proximity lists (closest locations per node)
    void initClosestLocations();


public:
    PDPTWData();
    PDPTWData(PDPTWData const &rhs) = delete;
    PDPTWData(PDPTWData &&rhs) noexcept;
    PDPTWData &operator=(PDPTWData &&rhs) noexcept;
    PDPTWData &operator=(PDPTWData const &rhs) = delete;
    ~PDPTWData() = default;

    /**
     * Constructs a basic PDPTWData instance without elevation segmentation.
     */
    PDPTWData(std::string dataName, int size, int capacity, Location depot, std::vector<Location> locations,
              Matrix costMatrix);

    /**
     * Constructs a PDPTWData instance including segmented slope and distance matrices.
     */
    PDPTWData(std::string dataName, int size, int capacity, Location depot, std::vector<Location> locations,
              Matrix costMatrix, Matrix timeMatrix, std::vector<std::vector<std::vector<double>>> segmentSlopeMatrix,
              std::vector<std::vector<std::vector<double>>> segmentDistanceMatrix);

    /**
     * Performs consistency checks on the data.
     */
    void checkData() const;

    /**
     * Verifies that the cost matrix is square and (optionally) consistent.
     * Returns true if errors are found.
     */
    bool checkMatrix() const;

    /**
     * Validates locations, including correct pairing and time window logic.
     * Returns true if inconsistencies are found.
     */
    bool checkLocation() const;


    std::vector<Location> const &getLocations() const;
    int getLocationCount() const;
    std::vector<Pair> const &getPairs() const;
    int getPairCount() const;
    Pair const &getPair(int id) const;


    /**
     * Returns the location corresponding to the given ID.
     * ID = 0 returns the depot; otherwise, returns the appropriate customer location.
     */
    Location const &getLocation(int id) const;
    Location const &getDepot() const;

    Matrix const &getMatrix() const;

    std::vector<std::vector<std::vector<double>>> const &getSegmentDistanceMatrix() const;
    std::vector<std::vector<std::vector<double>>> const &getSegmentSlopeMatrix() const;

    /**
     * Returns a list of location IDs (excluding depot), sorted by increasing distance from the given location ID.
     */
    std::vector<int> const &getClosestLocationsID(int id) const;

    int getSize() const;
    int getCapacity() const;
    std::string getDataName() const;

    std::vector<std::vector<std::tuple<double, double>>> const &getPreCalculation() const;
    void setPreCalculation(std::vector<std::vector<std::tuple<double, double>>> &&penalties);

    void print() const;
};
