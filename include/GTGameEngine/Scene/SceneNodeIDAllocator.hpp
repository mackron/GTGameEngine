// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef GT_SceneNodeIDAllocator
#define GT_SceneNodeIDAllocator

#include <GTLib/Threading/Mutex.hpp>
#include <cstdint>

namespace GT
{
    /// Class for creating unique IDs for scene nodes.
    ///
    /// This should always be assigned with a Scene object and serialized/deserialized with it.
    ///
    /// Although the term "allocator" is used here, this does not ever allocate memory on the heap.
    ///
    /// ID's will be sequential, and when serialized/deserialized, will be persistent.
    class SceneNodeIDAllocator
    {
    public:

        /// Constructor.
        SceneNodeIDAllocator(uint64_t minID);


        /// Allocates and return new ID.
        ///
        /// @remarks
        ///     This is thread-safe.
        uint64_t AllocateID();


    private:

        /// The minimum ID. Must be larger than 0. Defaults to 1.
        uint64_t m_minID;

        /// The next ID.
        uint64_t m_nextID;

        /// Allocation lock for thread-safety.
        GTLib::Mutex m_lock;
    };
}

#endif
