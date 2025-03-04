
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
#include "input/data.h"
#include "lns/constraints/capacity/capacity_constraint.h"
#include "lns/constraints/time_window/time_window_constraint.h"
#include "lns/solution/solution.h"
#include "lns/modification/pair/insert_pair.h"
#include "lns/modification/route/insert_route.h"
#include "lns/modification/pair/remove_pair.h"
#include "lns/modification/route/remove_route.h"

using json = nlohmann::json;

int main(int argc, char const *argv[])
{
    //std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/test_inst.json"; 
    //std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/n100/bar-n100-1.json";
    std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/Nantes_1.json";
    //std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/n5000/bar-n5000-1.json";

    std::cout << filepath << "\n";

    PDPTWData data = parsing::parseJson(filepath);
    data.checkData();

    data.print();
    
    std::cout << " \n";
    /* 
    * test 
    */
    Solution emptySol = Solution::emptySolution(data);
    emptySol.print();

    return 0;
}
