
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
#include "lns/operators/generators/modification_generator.h"
#include "lns/operators/sorting_strategy.h"


using json = nlohmann::json;

void printForwardList(const std::forward_list<std::unique_ptr<AtomicRecreation>>& list) {
    int cpt = 1;
    for (const auto& item : list) {
        std::cout << cpt << "{ \n";
        // Affichage de l'emplacement ajouté à la solution
        const Pair* pair = item->getAddedPairs();
        if (pair != nullptr) {
            std::cout << "Added Location ID: \n";
            pair->print();
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

    sorting_strategy::Shuffle shuffleStrategy(solution);  // Créer une instance de la stratégie de tri
    auto sortedPairs = shuffleStrategy.sortPairs();

    for (auto id : sortedPairs)
    {
        std::cout << id << " ";
    }
    std::cout << "\n";
    // ListHeuristicInsertion<sorting_strategy::SortingStrategy, generator::ModificationGenerator> operator;

    // double blinkRate = 0.1; // Par exemple, 10% de probabilité d'ignorer l'insertion
    // operator.reconstructSolution(emptySol, blinkRate);

    std::cout << "---- \n";

    generator::AllTypedModifications<InsertPair> generator;
    generator::ModificationContainer modificationList;
    
   
    std::cout << "---- \n";

    int pairID = solution.getBank().front();
    Pair pair = solution.getData().getPair(pairID);

    pair.print();

    std::cout << "---- \n";
    generator.populate(solution, pair, modificationList); 

    // print the pair associated to each modif
    printForwardList(modificationList);
    
    std::cout << "\n ---- \n \n";
    //ListHeuristicInsertion<std::derived_from<sorting_strategy::SortingStrategy> Strategy, std::derived_from<generator::ModificationGenerator> Generator>

    double blinkRate = 0;

    //operator.reconstructSolution(solution, blinkRate);
    ListHeuristicInsertion<sorting_strategy::Shuffle, generator::AllTypedModifications<InsertPair>> operatorInstance;

    operatorInstance.reconstructSolution(solution, blinkRate);

    solution.print();
    return 0;
}
