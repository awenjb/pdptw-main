#pragma once

#include "input/pair.h"
#include "lns/modification/atomic_recreation.h"
#include "lns/modification/pair/insert_pair.h"

#include <forward_list>
#include <memory>

namespace generator
{
    using ModificationContainer = std::forward_list<std::unique_ptr<AtomicRecreation>>;

    /**
     * Abstract class to generate valid modifications
     */
    class ModificationGenerator
    {
    public:
        /**
          * Adds valid insertions of request to the solution in the list provided as argument
          */
        virtual void populate(Solution const &solution, Pair const &, ModificationContainer &modificationList) = 0;

        virtual ~ModificationGenerator() = default;
    };

    /**
     * Empty template class to generate ALL valid modification of a specific type
     * @tparam T the type of modification to generate
     */
    template<std::derived_from<AtomicRecreation> T>
    class AllTypedModifications : public ModificationGenerator
    {
        // this assert will always fail. Needs a template specification
        static_assert(sizeof(T) == 0, "The generator for type T has not been defined yet.");
    };

    template<>
    class AllTypedModifications<InsertPair> : public ModificationGenerator
    {
    public:
        void populate(Solution const &solution, Pair const &, ModificationContainer &list) override;
    };


}// namespace generator