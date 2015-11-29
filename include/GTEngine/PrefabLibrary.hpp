// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_PrefabLibrary_hpp_
#define __GTEngine_PrefabLibrary_hpp_

#include "Prefab.hpp"
#include <utility>

namespace GT
{
    /// Class representing the library of prefabs.
    class PrefabLibrary
    {
    public:

        //////////////////////////////////////
        // Startup/Shutdown.

        /// Starts up the library.
        static bool Startup();

        /// Shuts down the library.
        static void Shutdown();


        
        //////////////////////////////////////
        // Acquire/Unacquire

        /// Acquires a prefab by it's file name.
        ///
        /// @param fileName       [in] The file name of the prefab being loaded, relative to the data directory.
        /// @param makeRelativeTo [in] If 'fileName' is absolute, this will be used to turn it into a relative path.
        ///
        /// @return A pointer to the prefab, or null if an error occurs.
        ///
        /// @remarks
        ///     Each call to this function must match up to an Unacquire().
        ///     @par
        ///     All resources must have a relative path somewhere. If it doesn't, there will be errors with serialization. Thus,
        ///     this will return null if 'fileName' is absolute and 'makeRelativeTo' is null.
        static Prefab* Acquire(const char* fileName, const char* makeRelativeTo = nullptr);

        /// Unacquires a prefab that was first acquired by Acquire().
        ///
        /// @param prefabToUnacquire [in] A pointer to the scene node class to unacquire.
        static void Unacquire(const Prefab* prefabToUnacquire);
    };
}

#endif