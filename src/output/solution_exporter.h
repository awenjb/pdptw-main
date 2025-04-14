#pragma once

#include "lns/solution/solution.h"
#include "output/lns_output.h"

#include <ctime>
#include <fstream>
#include <nlohmann/json.hpp>

namespace output
{
    /**
     * Get a json representation of a solution with the same info as in the benchmarks results (Li&Lim, Sartori&Buriol)
     */
    nlohmann::ordered_json getMinimalJson(Solution const &solution);

    /**
     * Get a complete json representation of a solution (heavier)
     */
    nlohmann::ordered_json getCompleteJson(output::LnsOutput const &result);


    nlohmann::ordered_json routeToJson(int routeID, Route const &route);


    void exportToJson(output::LnsOutput const &result);

}// namespace output

std::string getCurrentDate();