
#include <nlohmann/json.hpp>
#include <vector>
#include <iostream>
#include <string>

#include <filesystem>
#include <fstream>

#include <spdlog/spdlog.h>

#include "input/location.h"
#include "input/time_window.h"
#include "input/json_parser.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

int main(int argc, char const *argv[])
{
    /* code */

    //std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/test_inst.json"; 
    std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/n100/bar-n100-1.json";
    //std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/n5000/bar-n5000-1.json";

    std::cout << filepath << "\n";

    PDPTWData data = parsing::parseJson(filepath);
    data.checkData();

    /* test */
    TimeWindow tw = TimeWindow(3,4);
    Location pos = Location(1, 1, 20, 3, tw, 4, 5, LocType::DEPOT);





    return 0;
}
