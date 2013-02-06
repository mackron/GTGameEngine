// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneNodeClassLibrary_hpp_
#define __GTEngine_SceneNodeClassLibrary_hpp_

#include "SceneNodeClass.hpp"

namespace GTEngine
{
    /// Class representing the library of scene node classes.
    class SceneNodeClassLibrary
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

        /// Acquires a scene node class by it's file name.
        ///
        /// @param fileName       [in] The file name of the class being loaded, relative to the data directory.
        /// @param makeRelativeTo [in] If 'fileName' is absolute, this will be used to turn it into a relative path.
        ///
        /// @return A pointer to the class, or null if an error occurs.
        ///
        /// @remarks
        ///     Each call to this function must match up to an Unacquire().
        ///     @par
        ///     All resources must have a relative path somewhere. If it doesn't, there will be errors with serialization. Thus,
        ///     this will return null if 'fileName' is absolute and 'makeRelativeTo' is null.
        static SceneNodeClass* Acquire(const char* fileName, const char* makeRelativeTo = nullptr);

        /// Unacquires a class that was first acquired by Acquire().
        ///
        /// @param sceneNodeClassToUnacquire [in] A pointer to the scene node class to unacquire.
        static void Unacquire(const SceneNodeClass* sceneNodeClassToUnacquire);
    };
}

#endif