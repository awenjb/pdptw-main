#pragma once
#include <string>

// Parameters

// Penalty for excluding a certain pickup-delivery pair, aims to minimize the number of excluded pairs.
int const EXCLUSION_PENALTY = 1000000;

// Penalty applied to a route, aims to minimize the number of routes.
int const ROUTE_PENALTY = 0;

// Number of vehicles involved at the start of the optimization process (cannot be increased).
int const NUMBER_VEHICLE = 50;

// Number of iterations for the algorithm or simulation to run.
int const NUMBER_ITERATION = 1000;

// Flags

// Flag indicating whether the random seed has been set (true means it is set).
bool const SEED_SET = true;

// Seed for random number generation (ensures reproducibility).
int const RANDOM_SEED = 100;

// Flag indicating whether we print during the execution.
bool const PRINT = true;

// Flag indicating whether the final solution is stored.
bool const STORE_SOLUTION = false;

// Flag indicating whether the solution is stored with complete information or not.
bool const COMPLETE_STORE = true;

// Directories

std::string const OUTPUT_DIRECTORY = "./../../output/select_lc103";