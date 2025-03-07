#pragma once

#include "lns/solution/solution.h"

#include <nlohmann/json.hpp>
#include <ctime> 
#include <fstream>


namespace output
{
    /**
     * Get a json representation of a solution with the same info as in the benchmarks results (Li&Lim, Sartori&Buriol)
     */
    nlohmann::ordered_json getMinimalJson(Solution const &solution);

    /**
     * Get a complete json representation of a solution (heavier)
     */
    nlohmann::ordered_json getCompleteJson(Solution const &solution);


    nlohmann::ordered_json routeToJson(int routeID, const Route& route);


    void exportToJson(Solution const &solution);

}// namespace output

std::string getCurrentDate();