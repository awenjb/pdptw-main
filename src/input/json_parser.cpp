#include "json_parser.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <utility>

// spdlog repris du code lns-framework mais pas utilisé ici -> TODO

namespace fs = std::filesystem;
using json = nlohmann::json;

bool checkFilePresence(std::string &filepath)
{
    return fs::is_regular_file(filepath);
}

PDPTWData parsing::parseJson(std::string filepath)
{
    if (!checkFilePresence(filepath))
    {
        spdlog::error("Data file \"{}\" does not exist", filepath);
        exit(1);
    }

    std::ifstream jsonFile(filepath);

    if (!jsonFile.is_open())
    {
        spdlog::error("Unable to open file: {}", filepath);
        spdlog::default_logger()->flush();
        exit(1);
    }

    try
    {
        // extract filename (data name)
        size_t pos = filepath.find_last_of("/\\");
        std::string filename = (pos != std::string::npos) ? filepath.substr(pos + 1) : filepath;

        size_t dotPos = filename.find_last_of('.');
        if (dotPos != std::string::npos)
        {
            filename = filename.substr(0, dotPos);
        }

        // generate PDPTWData
        json j;
        jsonFile >> j;
        return json_to_data(filename, j);
        
    } catch (std::exception const &e)// catching all exceptions to properly log the error and quit gracefully
    {
        spdlog::error("Error while parsing the input json:");
        spdlog::error("{}", e.what());
        spdlog::default_logger()->flush();
        exit(1);
    }
}

PDPTWData json_to_data(std::string dataName, json const &j)
{
    int size = j.at("size").get<int>();
    int capacity = j.at("capacity").get<int>();

    auto depot_json = j.at("depot");

    TimeWindow tw(depot_json.at("timeWindow").at(0), depot_json.at("timeWindow").at(1));
    Location depot(0,
                   depot_json.at("longitude"),
                   depot_json.at("latitude"),
                   0,
                   tw,
                   depot_json.at("serviceDuration"),
                   0,
                   LocType::DEPOT);

    std::vector<Location> locations;

    for (auto const &loc: j.at("locations"))
    {
        LocType loc_type = (loc.at("locType") == "PICKUP") ? LocType::PICKUP : LocType::DELIVERY;
        TimeWindow loc_tw(loc.at("timeWindow").at(0), loc.at("timeWindow").at(1));

        locations.emplace_back(loc.at("id"),
                               loc.at("longitude"),
                               loc.at("latitude"),
                               loc.at("demand"),
                               loc_tw,
                               loc.at("serviceDuration"),
                               loc.at("pairedLocation"),
                               loc_type);
    }

    Matrix distance_matrix = j.at("distance_matrix").get<Matrix>();

    return {std::move(dataName), size, capacity, depot, locations, distance_matrix};
}
