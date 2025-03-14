#pragma once

class Solution;
/**
 * see AcceptanceFunction::operator()
 */
enum class AcceptationStatus
{
    ACCEPT,
    REFUSE
};

/**
 * Function object that accept or not a solution as the new solution for the next LNS iteration.
 */
class AcceptanceFunction
{
public:
    virtual ~AcceptanceFunction() = default;
    /**
     * Indicate which solution must be used for the next iteration.
     * @param candidateSolution the solution that can be accepted
     * @param actualSolution the solution used to create the candidate solution
     * @param bestSolution the best known solution known to this point.
     * @return ACCEPT to use \p candidateSolution for next iteration, REFUSE to continue to use \p actualSolution.
     */
    virtual AcceptationStatus operator()(Solution const &candidateSolution, Solution const &actualSolution,
                                         Solution const &bestSolution) const = 0;
};