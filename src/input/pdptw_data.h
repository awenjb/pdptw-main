#pragma once

#include <nlohmann/json_fwd.hpp>
#include <vector>

#include "pair.h"
#include "location.h"
#include "types.h"

using json = nlohmann::json;

/**
 * Throw this exception after errors in the input has been found.
 */
class InputJsonException : public std::exception
{
    std::string reason;

public:
    explicit InputJsonException(std::string_view reason);
    char const *what() const noexcept override;
};

class PDPTWData
{
    int size;
    int capacity;
    Location depot;
    std::vector<Location> locations;
    std::vector<Pair> pairs; // std::unordered_map<int, Pair> pair; if getPair(index) is needed ?
    Matrix distanceMatrix;

public:
    PDPTWData();
    PDPTWData(PDPTWData const &rhs) = delete;
    PDPTWData(PDPTWData &&rhs) noexcept;
    PDPTWData &operator=(PDPTWData &&rhs) noexcept;
    PDPTWData &operator=(PDPTWData const &rhs) = delete;
    ~PDPTWData() = default;

    /**
     * Constructs an empty PDPTWData.
     * @see parsing::parseJson
     */
    PDPTWData(int size, int capacity, Location depot, std::vector<Location> locations, Matrix distanceMatrix);
    /**
     * Checks some data coherence
     */
    void checkData() const;
    bool checkMatrix() const;
    bool checkLocation() const;


    std::vector<Location> const &getLocations() const;
    std::vector<Pair> const &getPairs() const;

    const Pair &getPair(int id) const;

    /** 
    *   0 return the depot.
    *   Other numbers return the associated location.
    */
    Location const &getLocation(int id) const;
    Location const &getDepot() const;

    Matrix const &getMatrix() const;

    int getSize() const;
    int getCapacity() const;

    void print() const;
};
