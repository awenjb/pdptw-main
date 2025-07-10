#include "input/load_dependent.h"

#include "config.h"
#include "input/pdptw_data.h"

#include <cassert>
#include <math.h>
#include <vector>

/*
 *  Model giving the correct penalty value for
 *  t_ij^m = t_ij^0 * penaltyModel(weight_ij, slope_ij)
 */
double ltt::penaltyModel(double weight, double slope)
{
    static constexpr double penaltyCoefficients[][5] = {{1.0000, 0.0000, 0.0000, 0.0000, 0.0000},
                                                        {0.9918, -0.0005, -0.2217, 0.0019, 0.0596},
                                                        {1.0928, -0.0013, -0.2300, 0.0021, 0.0292},
                                                        {1.1901, -0.0023, -0.2297, 0.0023, 0.0189},
                                                        {1.1258, -0.0023, -0.1848, 0.0023, 0.0112},
                                                        {0.9793, -0.0018, -0.1358, 0.0023, 0.0065},
                                                        {0.8135, -0.0012, -0.0949, 0.0022, 0.0036},
                                                        {0.6668, -0.0007, -0.0664, 0.0022, 0.0020},
                                                        {0.5390, -0.0002, -0.0448, 0.0021, 0.0010},
                                                        {0.4539, 0.0002, -0.0337, 0.0021, 0.0007},
                                                        {0.3592, 0.0006, -0.0206, 0.0021, 0.0002}};

    int index = (slope < 0) ? 0 : ((slope >= 9) ? 10 : static_cast<int>(slope));
    auto const &c = penaltyCoefficients[index];

    return c[0] + c[1] * weight + c[2] * slope + c[3] * weight * slope + c[4] * slope * slope;
}

double ltt::fontaineMethod(double distance, double slope, double load)
{
    double speed = 25;

    double const available_power = 350;
    double const Cr = 0.01; // rolling coefficient
    double const g = 9.81;  // m/s²
    double const Cd = 1.18; // drag coefficient
    double const rho = 1.18;// air density kg/m³
    double const A = 0.83;  // frontal area in m²

    double slope_rad = atan(slope / 100);
    double m = load + WEIGHT;
    double v_ms = speed / 3.6;

    double F_rolling = Cr * g * m * cos(slope_rad);
    double F_gravity = g * m * sin(slope_rad);
    double F_air = 0.5 * Cd * rho * A * pow(v_ms, 2);

    double power = (F_rolling + F_gravity + F_air) * v_ms / 0.95;// efficiency

    while (power > available_power && speed > 0)
    {
        speed -= 0.01;

        v_ms = speed / 3.6;
        F_air = 0.5 * Cd * rho * A * pow(v_ms, 2);
        power = (F_rolling + F_gravity + F_air) * v_ms / 0.95;
    }
    return distance / v_ms;
}

double ltt::fontaineCalculation(std::vector<double> const &segmentDistance, std::vector<double> const &segmentSlope,
                                double load)
{
    double travelTime = 0.0;
    size_t size = segmentDistance.size();
    assert(segmentSlope.size() == size);// to be sure...

    for (std::size_t i = 0; i < size; ++i)
    {
        travelTime += fontaineMethod(segmentDistance.at(i), segmentSlope.at(i), load);
    }
    return travelTime;
}

void ltt::preCalculation(PDPTWData &data)
{
    auto const &sm = data.getSegmentSlopeMatrix();
    auto const &dm = data.getSegmentDistanceMatrix();
    auto const distanceMatrix = data.getMatrix();

    double const penaltyCoefficients[][5] = {
            {1.0000, 0.0000, 0.0000, 0.0000, 0.0000},  // slope < 0
            {0.9918, -0.0005, -0.2217, 0.0019, 0.0596},// 0 <= slope < 1
            {1.0928, -0.0013, -0.2300, 0.0021, 0.0292},// 1 <= slope < 2
            {1.1901, -0.0023, -0.2297, 0.0023, 0.0189},// 2 <= slope < 3
            {1.1258, -0.0023, -0.1848, 0.0023, 0.0112},// 3 <= slope < 4
            {0.9793, -0.0018, -0.1358, 0.0023, 0.0065},// 4 <= slope < 5
            {0.8135, -0.0012, -0.0949, 0.0022, 0.0036},// 5 <= slope < 6
            {0.6668, -0.0007, -0.0664, 0.0022, 0.0020},// 6 <= slope < 7
            {0.5390, -0.0002, -0.0448, 0.0021, 0.0010},// 7 <= slope < 8
            {0.4539, 0.0002, -0.0337, 0.0021, 0.0007}, // 8 <= slope < 9
            {0.3592, 0.0006, -0.0206, 0.0021, 0.0002}  // slope >= 9
    };
    // route data matrices are supposed to be square
    std::size_t size = sm.size();

    std::vector<double> V_t0(size, 0.0);
    std::vector<double> V_As(size, 0.0);
    std::vector<double> V_Bs(size, 0.0);

    std::vector<std::vector<std::tuple<double, double>>> preCalculation(
            size, std::vector<std::tuple<double, double>>(size, {0.0, 0.0}));

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            if (i != j)
            {
                std::vector<double> const &segment_distance = dm.at(i).at(j);
                std::vector<double> const &segment_slope = sm.at(i).at(j);

                size_t seg_size = segment_distance.size();

                double A_total = 0.0;
                double B_total = 0.0;

                for (int k = 0; k < seg_size; k++)
                {
                    double dist = segment_distance[k];
                    double slope = segment_slope[k];
                    double t0 = (dist * 3.6) / 25.0;

                    int slope_index = 0;
                    if (slope < 0)
                    {
                        slope_index = 0;
                    }
                    else if (slope >= 9)
                    {
                        slope_index = 10;
                    }
                    else
                    {
                        slope_index = static_cast<int>(slope);
                    }

                    double const *coef = penaltyCoefficients[slope_index];
                    double a = coef[0];
                    double b = coef[1];
                    double c = coef[2];
                    double d = coef[3];
                    double f = coef[4];

                    double As = a + c * slope + f * slope * slope;
                    double Bs = b + d * slope;

                    A_total += t0 * As;
                    B_total += t0 * Bs;
                }
                preCalculation[i][j] = std::make_tuple(A_total, B_total);
            }
        }
    }

    data.setPreCalculation(std::move(preCalculation));
}

double ltt::getTravelTimeLTT(PDPTWData const &data, double load, int from, int to)
{
    // double constantTime = data.getMatrix().at(from).at(to) / 6.94444;

    // double fontaineTime = ltt::fontaineCalculation(data.getSegmentDistanceMatrix().at(from).at(to),
    //                                                data.getSegmentSlopeMatrix().at(from).at(to),
    //                                                0);

    std::tuple<double, double> constants = data.getPreCalculation().at(from).at(to);
    return std::get<0>(constants) + std::get<1>(constants) * (WEIGHT + load);
}