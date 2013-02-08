// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_SceneNodeClassLibrary_hpp_
#define __GTEngine_SceneNodeClassLibrary_hpp_

#include "SceneNodeClass.hpp"
#include <utility>

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


#if 0
        /// Creates a new scene node class, returning false if it already exists.
        ///
        /// @param absolutePath [in] The absolute path to save the new class to.
        /// @param relativePath [in] The relative path to save the new class to.
        /// @param sceneNode    [in] The scene node to create the class from.
        ///
        /// @remarks
        ///     This does not acquire the class. Do that manually after creating.
        static bool Create(const char* absolutePath, const char* relativePath, const SceneNode &sceneNode);

        /// Updates a scene node class.
        ///
        /// @param absolutePath [in] The absolute path of the class to update.
        /// @param sceneNodes   [in] The list of scene nodes making up the prefab.
        /// @param hierarchy    [in] The hierarchy, containing indices to sceneNodes.
        ///
        /// @remarks
        ///     This returns false if the file does not exist.
        ///     @par
        ///     The second item in the pairs in sceneNodes if the local ID to use with the prefab. When set to 0, a new ID will be generated. The very
        ///     first ID must be 1, which is the root ID.
        static bool Update(const char* absolutePath, const GTCore::Vector<std::pair<SceneNode*, uint64_t>> &sceneNodes, const GTCore::Map<size_t, size_t> &hierarchy);
#endif
    };
}

#endif