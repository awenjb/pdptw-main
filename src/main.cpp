
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

namespace fs = std::filesystem;
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

    /* 
    * test 
    */
    Solution emptySol = Solution(data);

    // SOLUTION 1-6-2-7 (time ; 104)

    Solution::RequestBank bank {3,4,5,8,9,10};

    std::vector<int> path {1,2,6,7};
    Route route = Route(path, 0);
    std::vector<Route> routes;
    routes.push_back(route);

    int totalCost = 0;
    
    Solution sol = Solution(data, bank, routes, totalCost);

    sol.print();


    // Couple pickup/delivery
    Location r2P = data.getLocations()[1];
    Location r2D = data.getLocations()[6];
    Location r3P = data.getLocations()[2];
    Location r3D = data.getLocations()[7];

    Pair pair2 = Pair(r2P, r2D);
    Pair pair3 = Pair(r3P, r3D);
    
    // Modification 
    InsertPair opInsert = InsertPair(0, 2, 2, pair3);

    /*

    CapacityConstraint cap_const = CapacityConstraint(sol);
    

    cap_const.initCapacities();
    cap_const.print();

    std::cout << "Is Valid ? : " << cap_const.checkVariant(opInsert) << "\n";

    cap_const.applyVariant(opInsert);

    cap_const.print();

    RemovePair opRemove = RemovePair(0, 2, 3, pair3);

    cap_const.applyVariant(opRemove);
    
    cap_const.print();
    */


    TimeWindowConstraint twconst = TimeWindowConstraint(sol);
    return 0;
}
