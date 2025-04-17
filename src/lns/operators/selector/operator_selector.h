#pragma once

#include "lns/operators/abstract_operator.h"
#include <algorithm>
#include <memory>
#include <random>
#include <utility>
#include <vector>

/**
 *  Represent a pair (destruction and reconstruction) of operators.
 */
class OperatorPair
{
    DestructionOperator &destruct;
    ReconstructionOperator &reconstruct;
    /**
     * Allow the operator pair to force to accept the solution that will be created with the last getOperatorPair();
     * @return true to force the acceptance of the solution after the operators calls.
     */
    bool forceAcceptance;

public:
    OperatorPair(DestructionOperator &destruct, ReconstructionOperator &reconstruct, bool forceAcceptance = false, bool largeIteration = false);
    DestructionOperator &destructor();
    ReconstructionOperator &reconstructor();
    // the solution obtained with this operator pair will be accepted by the lns
    void setForceAcceptance();
    bool forceTakeSolution() const;
};


/**
 *  Abstract class which defines an interface for an operator selector
 */
class OperatorSelector
{
public:
    /**
     * Called once per iteration, defines which destruction reconstruction pair is going to be used.
     */
    virtual OperatorPair getOperatorPair() = 0;
    /**
     * Callback if a better solution has been found by the algorithm
     */
    virtual void betterSolutionFound() = 0;
    virtual ~OperatorSelector() = default;
};

/**
 *  Simple operator selector with weighted random distribution
 */
class SimpleOperatorSelector : public OperatorSelector
{
    std::vector<std::unique_ptr<DestructionOperator>> destructOperators;
    std::vector<std::unique_ptr<ReconstructionOperator>> reconstructOperators;
    std::discrete_distribution<> destructDistribution;
    std::discrete_distribution<> reconstructDistribution;

public:
    SimpleOperatorSelector() = default;
    SimpleOperatorSelector(SimpleOperatorSelector &rhs) = delete;
    SimpleOperatorSelector &operator=(SimpleOperatorSelector &rhs) = delete;
    SimpleOperatorSelector(SimpleOperatorSelector &&rhs) = default;
    SimpleOperatorSelector &operator=(SimpleOperatorSelector &&rhs) noexcept = default;

    template<std::derived_from<DestructionOperator> T>
    void addDestructor(T destructor, int weight = 1);

    template<std::derived_from<ReconstructionOperator> T>
    void addReconstructor(T reconstructor, int weight = 1);

    OperatorPair getOperatorPair() override;

    void betterSolutionFound() override {}
};



template<std::derived_from<DestructionOperator> T>
void SimpleOperatorSelector::addDestructor(T destructor, int weight)
{
    auto newDistribution = std::vector<double>(destructDistribution.probabilities());
    int deconstructCount = destructOperators.size();
    newDistribution.resize(deconstructCount);
    std::ranges::transform(
            newDistribution, newDistribution.begin(), [&](double elmt) { return elmt * deconstructCount; });
    newDistribution.push_back(weight);
    destructDistribution = std::discrete_distribution<>(newDistribution.begin(), newDistribution.end());
    destructOperators.push_back(std::make_unique<T>(std::move(destructor)));
}

template<std::derived_from<ReconstructionOperator> T>
void SimpleOperatorSelector::addReconstructor(T reconstructor, int weight)
{
    auto newDistribution = std::vector<double>(reconstructDistribution.probabilities());
    int reconstructCount = reconstructOperators.size();
    newDistribution.resize(reconstructCount);
    std::ranges::transform(
            newDistribution, newDistribution.begin(), [&](double elmt) { return elmt * reconstructCount; });
    newDistribution.push_back(weight);
    reconstructDistribution = std::discrete_distribution<>(newDistribution.begin(), newDistribution.end());
    reconstructOperators.push_back(std::make_unique<T>(std::move(reconstructor)));
}