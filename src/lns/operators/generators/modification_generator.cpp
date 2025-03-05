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
        std::cout << "avant error \n";
        return [&](ModificationType &&modification) {
            std::cout << "Check Modif \n";
            if (solution.checkModification(modification))
            {
                std::cout << "insert modif to list \n";
                list.push_front(std::make_unique<ModificationType>(modification));
            }
        };
    }

    void AllTypedModifications<InsertPair>::populate(Solution const &solution, Pair const &pair,
                                                     std::forward_list<std::unique_ptr<AtomicRecreation>> &list)
    {
        std::cout << "dans modif_generator \n";
        enumeration::enumerateAllInsertPair(
                solution, pair, addToListIfValidTemplate<InsertPair>(solution, list));
    }

}// namespace generator
