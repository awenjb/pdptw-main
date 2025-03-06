#include "modification_generator.h"

#include "enumerate.h"

namespace generator
{
    /**
     *
     * @tparam ModificationType the type of modification to be checked
     * @param solution the solution to check the modification validity
     * @param list the modification will be added to this list if valid
     * @return a function that takes a ModificationType and add it to list iff it is valid
     */
    template<std::derived_from<AtomicRecreation> ModificationType>
    std::function<void(ModificationType &&)> addToListIfValidTemplate(Solution const &solution,
                                                                      ModificationContainer &list)
    {
        return [&](ModificationType &&modification) {
            if (solution.checkModification(modification))
            {
                std::cout << " => Insert Modification" << "\n";
                list.push_front(std::make_unique<ModificationType>(modification));
            }
        };
        std::cout << " => Insert Modification" << "\n";
    }

    void AllTypedModifications<InsertPair>::populate(Solution const &solution, Pair const &pair,
                                                     std::forward_list<std::unique_ptr<AtomicRecreation>> &list)
    {
        std::cout << "\n MODIFICATION GENERATOR : \n";
        enumeration::enumerateAllInsertPair(
                solution, pair, addToListIfValidTemplate<InsertPair>(solution, list));
    }

}// namespace generator
