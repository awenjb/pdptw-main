
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
#include "lns/solution/solution.h"
#include "lns/modification/pair/insert_pair.h"
#include "lns/modification/route/insert_route.h"
#include "lns/modification/pair/remove_pair.h"
#include "lns/modification/route/remove_route.h"

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

    /* 
    * test 
    */

    // Empty Solution
    Solution sol = Solution(data);
    
    InsertRoute op1 = InsertRoute();

    // Ajoute une route vide
    op1.modifySolution(sol);

    Location r1P = data.getLocations()[0];
    Location r1D = data.getLocations()[50];
    Location r2P = data.getLocations()[1];
    Location r2D = data.getLocations()[51];
    Location r3P = data.getLocations()[2];
    Location r3D = data.getLocations()[52];

    // Opération : Ajoute une paire pickup delivery à la route 0 en position 0 et 0
    InsertPair op2 = InsertPair(0, 0, 0, r1P, r1D);
    op2.modifySolution(sol);
    InsertPair op3 = InsertPair(0, 0, 2, r2P, r2D);
    op3.modifySolution(sol);
    std::cout << "--- \n";

    data::routeCost(data, sol.getRoute(0));

    InsertPair op4 = InsertPair(0, 3, 4, r3P, r3D);
    std::cout << "cout d'insert : "<< op4.evaluate(sol) << "\n";
    op4.modifySolution(sol);
    
    std::cout << "--- \n";
    sol.print();
    data::routeCost(data, sol.getRoute(0));

    // TO DO
    // Test supr
    std::cout << "--- \n";
    RemovePair op5 = RemovePair(0, 3, 5, r3P, r3D);

    std::cout << "cout de supr : "<< op5.evaluate(sol) << "\n";

    op5.modifySolution(sol);
    sol.print();

    data::routeCost(data, sol.getRoute(0));

    RemoveRoute op6 = RemoveRoute(0);

    std::cout << "cout de supr : "<< op6.evaluate(sol) << "\n";

    op6.modifySolution(sol);

    sol.print();

    return 0;
}
