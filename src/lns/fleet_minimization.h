#pragma once

#include "lns_runtime_data.h"


/**
 * Calculates the sum of absence counters for all requests in the solution's bank.
 * @param solution The solution whose bank is being evaluated.
 * @param absCounter A vector storing the absence count for each request.
 * @return The total absence count for the bank requests.
 */
int sumAbs(Solution const &solution, std::vector<int> const &absCounter);

/**
 * Removes the route with the minimal sum of absence counters from the solution.
 * @param solution The solution from which to remove the route.
 * @param absCounter A vector storing the absence count for each request.
 */
void removeOneRoute(Solution &solution, std::vector<int> const &absCounter);

/**
 * Performs the fleet minimization process using string removal operators.
 * This function tries to iteratively improve the current solution by applying pairs of 
 * destruction and reconstruction operators. It tracks improvements in terms of route 
 * minimization and maintains the best found solution.
 * @param runtime Runtime data used to track time, iterations, and the best solution.
 * @param currentSolution The current solution to be optimized.
 */
void fleetMinimizationCVB(LnsRuntimeData& runtime, Solution& currentSolution);



