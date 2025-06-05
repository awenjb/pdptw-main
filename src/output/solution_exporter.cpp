#include "solution_exporter.h"

#include "config.h"

std::string getCurrentDate()
{
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    char buffer[11];// "DD-MM-YYYY" + null terminator
    std::strftime(buffer, sizeof(buffer), "%d-%m-%Y", &tm);
    return std::string(buffer);
}

nlohmann::ordered_json output::getMinimalJson(Solution const &solution)
{
    nlohmann::ordered_json jsonSolution;

    nlohmann::ordered_json jsonRoutes = nlohmann::ordered_json::array();
    int routeID = 0;
    for (auto const &route: solution.getRoutes())
    {
        jsonRoutes.push_back(routeToJson(routeID, route));
        ++routeID;
    }

    jsonSolution["instanceName"] = solution.getData().getDataName();
    jsonSolution["authors"] = "...";
    jsonSolution["date"] = getCurrentDate();
    jsonSolution["reference"] = "...";
    jsonSolution["routes"] = jsonRoutes;

    return jsonSolution;
}

nlohmann::ordered_json output::getCompleteJson(output::LnsOutput const &result)
{
    nlohmann::ordered_json jsonSolution;

    Solution solution = result.getBestSolution();

    nlohmann::ordered_json jsonRoutes = nlohmann::ordered_json::array();
    int routeID = 0;
    for (auto const &route: solution.getRoutes())
    {
        jsonRoutes.push_back(routeToJson(routeID, route));
        ++routeID;
    }

    jsonSolution["instanceName"] = solution.getData().getDataName();
    jsonSolution["authors"] = "...";
    jsonSolution["date"] = getCurrentDate();
    jsonSolution["reference"] = "...";
    jsonSolution["vehicles"] = solution.getNumberOfRoutes();
    jsonSolution["cost"] =  std::ceil(solution.getRawCost() * 100.0) / 100.0;
    // jsonSolution["total_distance"] = std::ceil(solution.getRawCost() * 100.0) / 100.0;
    // jsonSolution["total_duration"] = std::ceil(solution.getRawCost() * 100.0) / 100.0;
    jsonSolution["time(s)"] = result.getTimeSpent();
    jsonSolution["iteration"] = result.getNumberOfIteration();
    jsonSolution["unfullfilled"] = solution.getPairBank().size();
    jsonSolution["routes"] = jsonRoutes;

    jsonSolution["timeInFleetMin"] = result.getTimeSpentFleetMin();
    jsonSolution["iterationInFleetMin"] = result.getNumberOfIterationFleetMin();
    jsonSolution["timeBestInFleetMin(s)"] = result.getBestTimeFleetMin();
    jsonSolution["iterationBestInFleetMin"] = result.getBestIterationFleetMin();

    jsonSolution["bestVehicles"] = result.getBestVehicles();
    jsonSolution["bestCosts"] = result.getBestCosts();
    jsonSolution["bestTimes(ms)"] = result.getBestTimes();
    jsonSolution["bestIterations"] = result.getBestIterations();


    return jsonSolution;
}

void output::exportToJson(output::LnsOutput const &result)
{
    std::string directory = OUTPUT_DIRECTORY;
    std::string filename = directory + "/" + result.getBestSolution().getData().getDataName() + "_sol.json";

    if (!std::filesystem::exists(directory))
    {
        std::filesystem::create_directory(directory);
    }

    std::ofstream file(filename);
    if (!file)
    {
        spdlog::error("Error when opening the file {}", filename);
        exit(1);
        return;
    }

    nlohmann::ordered_json jsonData;
    if (COMPLETE_STORE)
    {
        jsonData =
                output::getCompleteJson(result);
    }
    else
    {
        jsonData = output::getMinimalJson(result.getBestSolution());
    }

    file << jsonData.dump(4);
    file.close();
    spdlog::info("Solution of {} exported to {}", result.getBestSolution().getData().getDataName(), OUTPUT_DIRECTORY);
}

nlohmann::ordered_json output::routeToJson(int routeID, Route const &route)
{
    return nlohmann::ordered_json{{"routeID", routeID}, {"locationIDs", route.getRoute()}};
}
