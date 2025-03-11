
#include <nlohmann/json.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>

#include "input/location.h"
#include "input/pdptw_data.h"
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

#include "lns/operators/reconstruction/list_heuristic_insertion.h"
#include "lns/operators/reconstruction/list_heuristic_insertion.hpp"
#include "lns/operators/sorting_strategy.h"

#include "lns/operators/reconstruction/enumerate.h"

#include "output/solution_exporter.h"

using json = nlohmann::json;

void printForwardList(const std::forward_list<std::unique_ptr<AtomicRecreation>>& list) {
    int cpt = 1;
    for (const auto& item : list) {
        std::cout << cpt << "{ \n";
        // Affichage de l'emplacement ajouté à la solution
        int pair = item->getAddedPairs();
        if (pair > 0) {
            std::cout << "Added Location ID: \n";
            std::cout << pair << "\n";
        } else {
            std::cout << "No location added.\n";
        }
        std::cout << "} \n";
        ++cpt;
    }
}

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
    Solution solution = Solution::emptySolution(data);

    std::cout << "--- Empty Solution --- \n";
    solution.print();

    double blinkRate = 0;
    SortingStrategyType strategy = SortingStrategyType::SHUFFLE;
    EnumerationType enumeration = EnumerationType::ALL_INSERT_PAIR;

    std::cout << "\n --- Operator - SHUFFLE - ALL_INSERTPAIR  -> reconstruction (NO COST UPDATE)\n";

    ListHeuristicInsertion heuristicInsertion;
    heuristicInsertion.reconstructSolution(solution, blinkRate, strategy, enumeration);

    solution.print();

    output::exportToJson(solution);
    return 0;
}
