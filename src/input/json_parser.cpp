#include "json_parser.h"

#include "config.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <utility>
#include <vector>

namespace fs = std::filesystem;
using json = nlohmann::json;

// UTILITY

bool checkFilePresence(std::string const &filepath)
{
    return fs::is_regular_file(filepath);
}

std::string getFilenameWithoutExtension(std::string const &path)
{
    auto pos = path.find_last_of("/\\");
    auto filename = (pos != std::string::npos) ? path.substr(pos + 1) : path;
    auto dotPos = filename.find_last_of('.');
    return (dotPos != std::string::npos) ? filename.substr(0, dotPos) : filename;
}

Location parseDepot(json const &depot_json)
{
    TimeWindow tw(depot_json.at("timeWindow")[0], depot_json.at("timeWindow")[1]);
    return {0,
            depot_json.at("longitude"),
            depot_json.at("latitude"),
            0,
            tw,
            depot_json.at("serviceDuration"),
            0,
            LocType::DEPOT};
}

Location parseLocation(json const &loc_json)
{
    LocType loc_type = (loc_json.at("locType") == "PICKUP") ? LocType::PICKUP : LocType::DELIVERY;
    TimeWindow loc_tw(loc_json.at("timeWindow")[0], loc_json.at("timeWindow")[1]);

    return {loc_json.at("id"),
            loc_json.at("longitude"),
            loc_json.at("latitude"),
            loc_json.at("demand"),
            loc_tw,
            loc_json.at("serviceDuration"),
            loc_json.at("pairedLocation"),
            loc_type};
}

// PARSING

PDPTWData parsing::parseJson(std::string const &filepath)
{
    if (!checkFilePresence(filepath))
    {
        spdlog::error("Data file \"{}\" does not exist", filepath);
        std::exit(EXIT_FAILURE);
    }

    std::ifstream jsonFile(filepath);
    if (!jsonFile)
    {
        spdlog::error("Unable to open file: {}", filepath);
        spdlog::default_logger()->flush();
        std::exit(EXIT_FAILURE);
    }

    try
    {
        json j;
        jsonFile >> j;
        const std::string filename = getFilenameWithoutExtension(filepath);

        return ELEVATION ? json_to_data_with_elevation(filename, j) : json_to_data(filename, j);

    } catch (std::exception const &e)
    {
        spdlog::error("Error while parsing the input JSON: {}", e.what());
        spdlog::default_logger()->flush();
        std::exit(EXIT_FAILURE);
    }
}

// JSON TO DATA

PDPTWData json_to_data(std::string const &dataName, json const &j)
{
    int size = j.at("size");
    int capacity = j.at("capacity");

    Location depot = parseDepot(j.at("depot"));

    std::vector<Location> locations;
    for (auto const &loc: j.at("locations"))
    {
        locations.emplace_back(parseLocation(loc));
    }

    Matrix distance_matrix = j.at("distance_matrix").get<Matrix>();

    return {dataName, size, capacity, depot, locations, distance_matrix};
}

PDPTWData json_to_data_with_elevation(std::string const &dataName, json const &j)
{
    int size = j.at("size");
    int capacity = j.at("capacity");

    Location depot = parseDepot(j.at("depot"));

    std::vector<Location> locations;
    for (auto const &loc: j.at("locations"))
    {
        locations.emplace_back(parseLocation(loc));
    }

    Matrix distance_matrix(size, std::vector<double>(size, 0.0));
    Matrix time_matrix(size, std::vector<double>(size, 0.0));

    std::vector<std::vector<std::vector<double>>> segment_slope_matrix;
    std::vector<std::vector<std::vector<double>>> segment_distance_matrix;

    // Read the Path Information Matrix
    int i = 0;
    for (auto const &row: j.at("matrix"))
    {
        std::vector<std::vector<double>> slope_row;
        std::vector<std::vector<double>> dist_row;
        int j = 0;
        for (auto const &path_info: row)
        {
            // vector associated with the discretization of the slopes
            std::vector<double> slope_vec(21);// slopes from -10 to 10
            std::iota(slope_vec.begin(), slope_vec.end(), -10);


            // vecteur contenant la distance pour chaque intervalle de pente
            std::vector<double> dist_vec = path_info.at("slopes");

            double distance = path_info.at("distance");

            distance_matrix[i][j] = distance;
            time_matrix[i][j] = distance / 6.94444;// 25 km/h ≈ 6.94444 m/s

            slope_row.push_back(slope_vec);
            dist_row.push_back(dist_vec);

            j++;
        }

        segment_slope_matrix.push_back(slope_row);
        segment_distance_matrix.push_back(dist_row);

        i++;
    }

    return {dataName, size, capacity, depot, locations, distance_matrix, time_matrix, segment_slope_matrix, segment_distance_matrix};
}
