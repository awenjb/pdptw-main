#include "constraint.h"
#include "lns/solution/solution.h"

Constraint::Constraint(Solution const &solution) : solution(solution) {}

Solution const &Constraint::getSolution() const
{
    return solution.get();
}

PDPTWData const &Constraint::data() const
{
    return getSolution().getData();
}

void Constraint::setSolution(Solution const &newSolutionOwner)
{
    solution = newSolutionOwner;
}


void ModificationVisitor::applyVariant(ModificationApplyVariant const &applyModificationVariant)
{
    // lambda auto&& + std::visit combo
    // allows to split the variant to each strongly typed function for each modification (see the protected pure virtual functions apply)
    // If compilation breaks here, it is likely that a type T in the ModificationApplyVariant has no function ModificationVisitor::apply(T const &) associated
    std::visit([this](auto &&op) { this->apply(op); }, applyModificationVariant);   
}

bool ConstraintVisitor::checkVariant(ModificationCheckVariant const &checkModificationVariant)
{
    // lambda auto&& + std::visit combo
    // allows to split the variant to each strongly typed function for each modification (see the protected pure virtual functions apply)
    // If compilation breaks here, it is likely that a type T in the ModificationCheckVariant has no function ConstraintVisitor::apply(T const &) associated
    return std::visit([this](auto &&op) { return this->check(op); }, checkModificationVariant);
}
