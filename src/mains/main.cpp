
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
#include "lns/operators/destruction/random_destroy.h"
#include "lns/operators/reconstruction/list_heuristic_cost_oriented.h"
#include "lns/operators/selector/operator_selector.h"
#include "lns/operators/selector/small_large_selector.h"
#include "lns/solution/solution.h"
#include "lns/modification/pair/insert_pair.h"
#include "lns/modification/route/insert_route.h"
#include "lns/modification/pair/remove_pair.h"
#include "lns/modification/route/remove_route.h"

#include "lns/operators/reconstruction/list_heuristic_insertion.h"
#include "lns/operators/sorting_strategy.h"

#include "lns/operators/reconstruction/enumerate.h"

#include "mains/main_interface.h"
#include "output/solution_exporter.h"

using json = nlohmann::json;

void simpleLNS(PDPTWData const &data, Solution &startingSolution)
{
    // lns parameters
    int requests = data.getPairCount();

    int pairs = requests * 2 / 100;
    int manyPairs = requests * 40 / 100;

    // threshold function to do


    // lns operators
    SimpleOperatorSelector smallSelector;
    addAllReconstructor(smallSelector);
    smallSelector.addDestructor(RandomDestroy(pairs));

    SimpleOperatorSelector largeSelector;
    addAllReconstructor(largeSelector);
    largeSelector.addDestructor(RandomDestroy(pairs));

    SimpleOperatorSelector veryLargeSelector;
    addAllReconstructor(veryLargeSelector);
    veryLargeSelector.addDestructor(RandomDestroy(pairs));

    SimpleOperatorSelector hugeSelector;
    addAllReconstructor(hugeSelector);
    hugeSelector.addDestructor(RandomDestroy(pairs));

    SimpleOperatorSelector lastSelector;
    addAllReconstructor(lastSelector);
    lastSelector.addDestructor(RandomDestroy(manyPairs));

    std::vector<SmallLargeOperatorSelector::StepSelector> selectors;
    selectors.emplace_back(500, std::move(smallSelector));
    selectors.emplace_back(1000, std::move(largeSelector));
    selectors.emplace_back(1000, std::move(veryLargeSelector));
    selectors.emplace_back(3000, std::move(hugeSelector));
    selectors.emplace_back(1, std::move(lastSelector));
    SmallLargeOperatorSelector smallLargeSelector(std::move(selectors));

    // run lns
    //lns::runLns(startingSolution, smallLargeSelector, acceptor);
}

int main(int argc, char const *argv[])
{
    //std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/n100/bar-n100-1.json";
    std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/Nantes_1.json";
    //std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/n5000/bar-n5000-1.json";




    ///////////////////////////////////////////////////////////////////////::

    // std::cout << filepath << "\n";

    // data.print();
    
    // std::cout << " \n";
    // /* 
    // * test 
    // */
    // Solution solution = Solution::emptySolution(data);

    // std::cout << "--- Empty Solution --- \n";
    // solution.print();

    // double blinkRate = 0;
    // SortingStrategyType strategy = SortingStrategyType::SHUFFLE;
    // EnumerationType enumeration = EnumerationType::ALL_INSERT_PAIR;

    // std::cout << "\n --- Operator - SHUFFLE - ALL_INSERTPAIR  -> reconstruction (NO COST UPDATE)\n";

    // ListHeuristicCostOriented heuristic;
    // heuristic.reconstructSolution(solution, blinkRate, strategy, enumeration);

    // RandomDestroy randomdestroy = RandomDestroy(4);
    // randomdestroy.destroySolution(solution);


    //output::exportToJson(solution);   
    return 0;
}
