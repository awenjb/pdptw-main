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

    jsonSolution["InstanceName"] = solution.getData().getDataName();
    jsonSolution["Authors"] = "...";
    jsonSolution["Date"] = getCurrentDate();
    jsonSolution["Reference"] = "...";
    jsonSolution["routes"] = jsonRoutes;

    return jsonSolution;
}

nlohmann::ordered_json output::getCompleteJson(Solution const &solution, int iteration, double time)
{
    nlohmann::ordered_json jsonSolution;

    nlohmann::ordered_json jsonRoutes = nlohmann::ordered_json::array();
    int routeID = 0;
    for (auto const &route: solution.getRoutes())
    {
        jsonRoutes.push_back(routeToJson(routeID, route));
        ++routeID;
    }

    jsonSolution["InstanceName"] = solution.getData().getDataName();
    jsonSolution["Authors"] = "...";
    jsonSolution["Date"] = getCurrentDate();
    jsonSolution["Reference"] = "...";
    jsonSolution["Cost"] =  std::ceil(solution.getRawCost() * 100.0) / 100.0;
    jsonSolution["Time"] = time;
    jsonSolution["Iteration"] = iteration;
    jsonSolution["Unfullfilled"] = solution.getPairBank().size();
    jsonSolution["routes"] = jsonRoutes;

    return jsonSolution;
}

void output::exportToJson(output::LnsOutput result)
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
                output::getCompleteJson(result.getBestSolution(), result.getNumberOfIteration(), result.getTimeSpent());
    }
    else
    {
        jsonData = output::getMinimalJson(result.getBestSolution());
    }

    file << jsonData.dump(4);
    file.close();
    std::cout << "Solution exported" << std::endl;
}

nlohmann::ordered_json output::routeToJson(int routeID, Route const &route)
{
    return nlohmann::ordered_json{{"routeID", routeID}, {"locationIDs", route.getRoute()}};
}
