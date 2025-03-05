cmake_minimum_required(VERSION 3.25)

project(pdptw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# link time optimisation (LTO) for release and release with debug info builds
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Finding dependencies
find_package(CLI11 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

add_executable(pdptw src/mains/main.cpp
                src/mains/main_interface.cpp
                src/input/data.cpp
                src/input/location.cpp
                src/input/pair.cpp
                src/input/pdptw_data.cpp
                src/input/time_window.cpp
                src/input/json_parser.cpp
                src/lns/solution/route.cpp 
                src/lns/solution/solution.cpp
                src/lns/constraints/constraint.cpp
                src/lns/constraints/capacity/capacity_constraint.cpp
                src/lns/constraints/time_window/time_window_constraint.cpp
                src/lns/modification/pair/insert_pair.cpp 
                src/lns/modification/pair/remove_pair.cpp
                src/lns/modification/route/insert_route_with_pair.cpp
                src/lns/modification/route/insert_route.cpp
                src/lns/modification/route/remove_route.cpp
                src/output/solution_checker.cpp
                src/lns/operators/sorting_strategy.cpp
                src/lns/operators/destruction/random_destroy.cpp
                src/lns/operators/generators/enumerate.cpp 
                src/lns/operators/generators/modification_generator.cpp 
                src/lns/operators/reconstruction/list_heuristic_insertion.hpp
                src/lns/operators/selector/operator_selection.cpp
                src/utils.cpp
                )

target_link_libraries(pdptw PRIVATE CLI11::CLI11 nlohmann_json::nlohmann_json spdlog::spdlog)

target_include_directories(pdptw PUBLIC ${CMAKE_SOURCE_DIR}/src)
