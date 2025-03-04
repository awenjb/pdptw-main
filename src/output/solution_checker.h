#pragma once

#include <stdexcept>

#include "input/pdptw_data.h"
#include "input/time_window.h"
#include "lns/solution/solution.h"


namespace checker
{

    /**
     *  Does a check full check on the solution.
     *  @param checkRequests, if false will not throw on missing requests
     */
    void checkAll(Solution const &, PDPTWData const &data, bool checkRequests = true);

    /**
     *  Check that the solution represent a PDPTW solution.
     *  No doublon, Ordered pickup and delivery, pickup and delivery in the same route.
     *  (don't check the constraints)
     */
    void checkSolutionCoherence(Solution const &sol, PDPTWData const &data);

    /**
     *  Check the capacity constraint.
     */
    void checkCapacity(Solution const &sol, PDPTWData const &data);

    /**
     *  Check the time windows constraint.
     *  (Maybe a better way to do it than reconstructing the ordo ?)
     */
    void checkTimeWindows(Solution const &sol, PDPTWData const &data);


    /**
     * This error means that the internal Solutions values are broken.
     * Errors like this are not linked to the model constraint.
     */
    class SolutionInternalError : public std::logic_error
    {
    public:
        SolutionInternalError(std::string reason, Solution const &);
    };
 
    /**
    * This exception means that the solution is not valid regarding the PDPTW problem
    */
    class SolutionConstraintError : public std::logic_error
    {
    public:
        SolutionConstraintError(std::string reason, Solution const &);
    };


}// namespace checker
