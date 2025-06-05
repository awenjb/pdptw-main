#pragma once
#include <string>

/**********************************************/
// Parameters

// Penalty for excluding a certain pickup-delivery pair, aims to minimize the number of excluded pairs.
int const EXCLUSION_PENALTY = 1000000;

// Penalty applied to a route, aims to minimize the number of routes.
int const ROUTE_PENALTY = 0;

// Number of vehicles involved at the start of the optimization process (cannot be increased).
int const NUMBER_VEHICLE = 50;

// Number of iterations for the algorithm or simulation to run.
int const NUMBER_ITERATION = 1000000;

// Number of seconds for the algorithm to run.
int const MAX_DURATION_SEC = 20;

// % of total time given to the first phase (route minimisation)
double const FIRST_PHASE_THRESHOLD = 0.1;

// % of NUMBER_ITERATION
// If no new best solution is found within LNS_FREQUENCY small iterations, then the next iteration will be a large iteration.
double const LNS_FREQUENCY = 0.05;


/**********************************************/
// Flags

// Execute a classical SLNS (a variant is execute if false)
bool const SLNS = true;

// Execute a route minimization algorithm before the SLNS
bool const TWO_PHASE_ALGORITHM = true;

// Flag indicating whether the random seed has been set (true means it is set).
bool const SEED_SET = true;

// Seed for random number generation (ensures reproducibility).
int const RANDOM_SEED = 100;

// Flag indicating whether the solution is printed at the end of the execution.
bool const PRINT = true;

// Flag indicating whether the solution is stored with complete information or not.
bool const COMPLETE_STORE = true;

// Flag indicating whether the final solution is stored.
bool const STORE_SOLUTION = false;


/**********************************************/
// Directories
std::string const OUTPUT_DIRECTORY = "./../../output";


/**********************************************/
// Load-dependent travel time

// Consider elevation (instances with elevation are necessary)
bool const ELEVATION = false;

// Weight bike + cyclist
double const WEIGHT = 140;