#pragma once

#include "location.h"
#include <functional>
#include <nlohmann/json_fwd.hpp>
#include <vector>

using json = nlohmann::json;
using Matrix = std::vector<std::vector<double>>; 

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
    unsigned int size;
    int capacity;
    Location depot;
    std::vector<Location> locations;
    Matrix distanceMatrix;


public:
    PDPTWData();
    PDPTWData(PDPTWData const &rhs) = delete;
    PDPTWData(PDPTWData &&rhs) noexcept;
    PDPTWData &operator=(PDPTWData &&rhs) noexcept;
    PDPTWData &operator=(PDPTWData const &rhs) = delete;

    /**
     * Constructs an empty PDPTWData.
     * @see parsing::parseJson
     */
    PDPTWData(unsigned int size, int capacity, Location depot, std::vector<Location> requests, Matrix distanceMatrix);
    /**
     * Checks some data coherence
     */
    void checkData() const;
    bool checkMatrix() const;
    bool checkLocation() const;

    
    std::vector<Location> const &getLocations() const;
    Location const &getLocation(int id) const;
    Location const &getDepot() const;
    Matrix const &getMatrix() const;

    unsigned int getSize();
    int getCapacity();

    void print() const;
};
