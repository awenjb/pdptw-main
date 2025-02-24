cmake_minimum_required(VERSION 3.25)

project(pdptw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# link time optimisation (LTO) for release and release with debug info builds
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)


# Finding dependencies
find_package(CLI11 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)





add_executable(pdptw src/main.cpp 
                src/input/data.cpp
                src/input/location.cpp
                src/input/pdptw_data.cpp
                src/input/time_window.cpp
                src/input/json_parser.cpp
                src/lns/solution/route.cpp 
                src/lns/solution/solution.cpp
                src/lns/modification/pair/insert_pair.cpp 
                src/lns/modification/pair/remove_pair.cpp
                src/lns/modification/route/insert_route_with_pair.cpp
                src/lns/modification/route/insert_route.cpp
                src/lns/modification/route/remove_route.cpp
                )

target_link_libraries(pdptw PRIVATE CLI11::CLI11 nlohmann_json::nlohmann_json spdlog::spdlog)