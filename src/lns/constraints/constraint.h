#pragma once

#include <memory>
#include <stdexcept>
#include <variant>

// forward declaration of all modifications
class InsertPair;
class RemovePair;
class InsertRoute;
class RemoveRoute;
class Solution;
class PDPTWData;


using ModificationApplyVariant =
        std::variant<std::reference_wrapper<InsertPair const>, std::reference_wrapper<InsertRoute const>,
                     std::reference_wrapper<RemovePair const>, std::reference_wrapper<RemoveRoute const>>;

// defines the variant that can store all the modifications that can be checked (recreation modifications)
using ModificationCheckVariant =
        std::variant<std::reference_wrapper<InsertPair const>, std::reference_wrapper<InsertRoute const>,
                     std::reference_wrapper<RemovePair const>, std::reference_wrapper<RemoveRoute const>>;


/**
 * Visitor pattern for the modifications.
 * It splits the variant into separate apply functions for each Modification.
 */
class ModificationVisitor
{
protected:
    // A child class must handle all the modification types ie. implement all these functions
    virtual void apply(InsertPair const &op) = 0;
    virtual void apply(InsertRoute const &op) = 0;
    virtual void apply(RemovePair const &op) = 0;
    virtual void apply(RemoveRoute const &op) = 0;

public:
    /**
     * A modification has been applied to the solution,
     * the constraint can use information from the modification to update its state.
     */
    void applyVariant(ModificationApplyVariant const &applyModificationVariant);
};


/**
 * Visitor pattern for the constraint. Adds the check part that constraints must implement
 * It splits the variant into separate check/apply functions for each Modification.
 */
class ConstraintVisitor : public ModificationVisitor
{
protected:
    // A child class must handle all the modification types ie. implement all these functions
    virtual bool check(InsertPair const &op) const = 0;
    virtual bool check(InsertRoute const &op) const = 0;
    virtual bool check(RemovePair const &op) const = 0;
    virtual bool check(RemoveRoute const &op) const = 0;


public:
virtual ~ConstraintVisitor() = default;
    /**
     * Checks that a modification is valid for the solution
     * If a check fails it must return false.
     */
    bool checkVariant(ModificationCheckVariant const &checkModificationVariant);
};


/**
 * Base class for a constraint.
 * It has to define all checks (from ConstraintVisitor),
 * and can modify its state with the apply (from ModificationVisitor), it looks like a listener pattern
 */
class Constraint : public ConstraintVisitor
{
    std::reference_wrapper<Solution const> solution;

protected:
    explicit Constraint(Solution const &);
    PDPTWData const &data() const;

public:
    /**
     * It is expected that the new Solution is in valid state regarding precedence graph and times when cloning
     */
    virtual std::unique_ptr<Constraint> clone(Solution const &newOwningSolution) const = 0;

    Solution const &getSolution() const;

    // used when a solution is moved, we must update the owner
    void setSolution(Solution const &newSolutionOwner);
     ~Constraint() override = default;

    /**
     * Callback when many destruction are done. It is expected that checks work after a call to this.
     * Defaults to no op
    */
    virtual void endOfDestructionCallback() {}
};