
#include "input/data.h"
#include "input/json_parser.h"
#include "input/location.h"
#include "input/pdptw_data.h"
#include "input/time_window.h"
#include "lns/acceptance/threshold_acceptance.h"
#include "lns/constraints/capacity/capacity_constraint.h"
#include "lns/constraints/time_window/forward_time_slack.h"
#include "lns/constraints/time_window/time_window_constraint.h"
#include "lns/lns.h"
#include "lns/modification/pair/insert_pair.h"
#include "lns/modification/pair/remove_pair.h"
#include "lns/modification/route/insert_route.h"
#include "lns/modification/route/remove_route.h"
#include "lns/operators/abstract_operator.h"
#include "lns/operators/destruction/clean_empty_route.h"
#include "lns/operators/destruction/random_destroy.h"
#include "lns/operators/destruction/string_removal.h"
#include "lns/operators/reconstruction/enumerate.h"
#include "lns/operators/reconstruction/list_heuristic_cost_oriented.h"
#include "lns/operators/selector/operator_selector.h"
#include "lns/operators/selector/small_large_selector.h"
#include "lns/operators/sorting_strategy.h"
#include "lns/solution/solution.h"
#include "mains/main_interface.h"
#include "output/solution_checker.h"
#include "output/solution_exporter.h"
#include "types.h"
#include "output/run.h"
#include "config.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

using json = nlohmann::json;

void simpleLNS(PDPTWData const &data, Solution &startingSolution)
{
    // lns parameters
    int requests = data.getPairCount();

    int pairs = requests * 2 / 100;
    int manyPairs = requests * 40 / 100;

    // threshold function
    ThresholdAcceptance acceptor(0.05);

    // lns operators
    SimpleOperatorSelector RandomDestroy_BestInsert;
    addAllReconstructor(RandomDestroy_BestInsert);
    RandomDestroy_BestInsert.addDestructor(RandomDestroy(pairs));
    RandomDestroy_BestInsert.addDestructor(StringRemoval(10, 10));

    SimpleOperatorSelector largeSelector;
    addAllReconstructor(largeSelector);
    largeSelector.addDestructor(RandomDestroy(manyPairs));
    largeSelector.addDestructor(StringRemoval(10, 10));


    std::vector<SmallLargeOperatorSelector::StepSelector> selectors;
    selectors.emplace_back(10, std::move(RandomDestroy_BestInsert));
    selectors.emplace_back(50, std::move(largeSelector));

    SmallLargeOperatorSelector smallLargeSelector(std::move(selectors));

    // run lns
    output::LnsOutput result = lns::runLns(startingSolution, smallLargeSelector, acceptor);

    if (STORE_SOLUTION)
    {
        output::exportToJson(result);
    }


    // Solution sol = result.getBestSolution();
    // CapacityConstraint capa = CapacityConstraint(sol);

    // capa.initRouteCapacities();
    // capa.initMaxCapacity();
    // capa.print();

    // RemovePair rem = RemovePair(0, 1, 4, sol.getData().getPair(1));

    

    // sol.applyDestructSolution(rem);
    // sol.print();
    // capa.updateMaxCapacity(sol.getData(), sol.getRoute(0));
    // capa.print();
    // int pickup = 8;
    // int delivery = 8;
    // std::cout << std::endl<< capa.checkModif(sol.getData().getPair(1), 0, pickup, delivery) << std::endl;
    // std::cout << std::endl<< capa.checkModif2(sol.getData().getPair(1), 0, pickup, delivery) << std::endl;

    // InsertPair add = InsertPair(0, pickup, delivery, sol.getData().getPair(1));
    // sol.applyRecreateSolution(add);

    // checker::checkAll(sol, sol.getData());

    // capa.updateMaxCapacity(sol.getData(), sol.getRoute(0));
    // capa.print();
}

int main(int argc, char **argv)
{
    //return mainInterface(argc, argv, &simpleLNS);

    ///////////////////////////////////////////////////////////////////////

    //std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/n100/bar-n100-1.json";
    //std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/pdp_100/lc106.json";
    //std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/Nantes_1.json";
    //std::string filepath = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/n5000/bar-n5000-1.json";

    // PDPTWData data = parsing::parseJson(filepath);
    // Solution startingSolution = Solution::emptySolution(data);
    // simpleLNS(data, startingSolution);

    std::string path = "/home/a24jacqb/Documents/Code/pdptw-main/data_in/selection";
    runAllInDirectory(path, simpleLNS);



    return 0;
}
