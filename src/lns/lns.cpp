#include "lns.h"

#include "lns/acceptance/acceptance_function.h"
#include "lns/operators/selector/operator_selector.h"
#include "output/solution_checker.h"

namespace
{
    bool isBetterSolution(Solution const &candidateSolution, Solution const &bestKnownSol)
    {
        return candidateSolution.getCost() < bestKnownSol.getCost();
    }

}// namespace

output::LnsOutput lns::runLns(Solution const &initialSolution, OperatorSelector &opSelector,
                         AcceptanceFunction const &acceptFunctor)
{
    /**
     * The solution is the base solution used to create the neighbor solution.
     * It is constant unless we accept the candidate solution.
     */
    Solution actualSolution = initialSolution;
    Solution bestSolution = initialSolution;

    int it = 10;
    while (it > 0)
    {
        /**
         * The solution on which we apply the operators.
         * It is discarded at the end of each loop if it is not accepted by the Acceptance Function.
         */
        Solution candidateSolution = actualSolution;


        // Chose operator pair
        auto destructReconstructPair = opSelector.getOperatorPair();
        // Apply operators
        destructReconstructPair.destructor().destroySolution(candidateSolution);
        destructReconstructPair.reconstructor().reconstructSolution(candidateSolution, 0.01);

        // Update best solution
        if (isBetterSolution(candidateSolution, bestSolution))
        {
            checker::checkAll(candidateSolution, candidateSolution.getData(), false);

            bestSolution = candidateSolution;
            opSelector.betterSolutionFound();
        }

        // Check if we use the candidate solution as the new actual solution
        // operator can force to take the new solution
        if (destructReconstructPair.forceTakeSolution() ||
            acceptFunctor(candidateSolution, actualSolution, bestSolution) == AcceptationStatus::ACCEPT)
        {
            actualSolution = std::move(candidateSolution);
        }
        --it;
    }
    auto result = output::LnsOutput(std::move(bestSolution), it, it);
    return result;
}