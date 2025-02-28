#include "input/json_parser.h"
#include "input/pdptw_data.h"
#include "main_interface.h"

#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

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
        Solution startingSolution = Solution::emptySolution(data);

        function(data, startingSolution);

    } 
    catch (const std::exception& e) 
    {
        spdlog::critical("Exception caught : {}", e.what());
        return 1;
    }

    spdlog::info("Fin");
    return 0;
}
