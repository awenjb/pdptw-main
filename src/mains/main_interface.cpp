#include "main_interface.h"

#include "config.h"
#include "input/json_parser.h"
#include "input/load_dependent.h"
#include "input/pdptw_data.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

void addAllReconstructor(SimpleOperatorSelector &selector)
{
    selector.addReconstructor(ListHeuristicCostOriented(SortingStrategyType::SHUFFLE, EnumerationType::ALL_INSERT_PAIR),
                              1);
    selector.addReconstructor(ListHeuristicCostOriented(SortingStrategyType::DEMAND, EnumerationType::ALL_INSERT_PAIR),
                              1);
    selector.addReconstructor(ListHeuristicCostOriented(SortingStrategyType::FAR, EnumerationType::ALL_INSERT_PAIR), 1);
    selector.addReconstructor(ListHeuristicCostOriented(SortingStrategyType::CLOSE, EnumerationType::ALL_INSERT_PAIR),
                              1);
    selector.addReconstructor(ListHeuristicCostOriented(SortingStrategyType::TWWIDTH, EnumerationType::ALL_INSERT_PAIR),
                              1);
    selector.addReconstructor(ListHeuristicCostOriented(SortingStrategyType::TWSTART, EnumerationType::ALL_INSERT_PAIR),
                              1);
    selector.addReconstructor(ListHeuristicCostOriented(SortingStrategyType::TWEND, EnumerationType::ALL_INSERT_PAIR),
                              1);
}

int mainInterface(int argc, char **argv, std::function<void(PDPTWData &, Solution &)> function)
{
    try
    {
        if (argc <= 1)
        {
            spdlog::error("No arguments provided. Expected: one command line argument.");
            throw std::runtime_error("Error : No arguments provided.");
        }

        if (argc > 2)
        {
            spdlog::error("Too many arguments provided. Expected: one command line argument.");
            throw std::runtime_error("Error : Too many arguments arguments provided.");
        }

        std::string filepath = argv[1];
        PDPTWData data = parsing::parseJson(filepath);

        // pre calculate some value in case of a load-dependent instances
        if (ELEVATION)
        {
            ltt::preCalculation(data);
        }

        Solution startingSolution = Solution::emptySolution(data);

        function(data, startingSolution);

    } catch (std::exception const &e)
    {
        spdlog::critical("Exception caught : {}", e.what());
        return 1;
    }

    spdlog::info("End");
    return 0;
}
