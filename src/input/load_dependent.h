#pragma once

#include "input/pdptw_data.h"

/**
 * Functions for Load-dependent travel time
 */
namespace ltt
{
    /*
     *  Fontaine Algorithm
     */
    double fontaineMethod(double distance, double slope, double load);

    /*
     *  Calculate the total travel time a path divided into several segment using Fontaine algorithm
     */
    double fontaineCalculation(std::vector<double> const &segment_distance, std::vector<double> const &segment_slope,
                               double load);

    /*
     *  Return the penalty value associated with the load and slope
     */
    double penaltyModel(double weight, double slope);

    /*
     *  Update data
     */
    void preCalculation(PDPTWData &data);

    /*
     *  Data needs to be initialized with preCalculation before using getTravelTimeLTT
     */
    double getTravelTimeLTT(PDPTWData const &data, double load, int from, int to);

}// namespace ltt