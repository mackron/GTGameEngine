// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ScriptLibrary_hpp_
#define __GTEngine_ScriptLibrary_hpp_

#include "ScriptDefinition.hpp"

namespace GTEngine
{
    /// Static class for managing the loading and unloading of scripts.
    ///
    /// Although this loads script that will eventually find their way into higher-level script objects, the library is not responsible
    /// for doing any of that kind of stuff.
    class ScriptLibrary
    {
    public:

        /////////////////////////////////////////////////
        // Startup/Shutdown

        /// Starts up the script library.
        static bool Startup();

        /// Shuts down the script library.
        static void Shutdown();



        /////////////////////////////////////////////////
        // Acquire/Unacquire

        /// Acquires a script definition.
        ///
        /// @param fileName       [in] The file name of the script being loaded, relative to the data directory.
        /// @param makeRelativeTo [in] If 'fileName' is absolute, this will be used to turn it into a relative path.
        ///
        /// @return A pointer to the script definition, or null if an error occurs.
        ///
        /// @remarks
        ///     Each call to this function must match up to an Unacquire().
        ///     @par
        ///     All resources must have a relative path somewhere. If it doesn't, there will be errors with serialization. Thus,
        ///     this will return null if 'fileName' is absolute and 'makeRelativeTo' is null.
        static ScriptDefinition* Acquire(const char* fileName, const char* makeRelativeTo = nullptr);

        /// Unacquires a script definition that was first acquired by Acquire().
        ///
        /// @param scriptDefinitionToUnacquire [in] A pointer to the script definition to unacquire.
        static void Unacquire(const ScriptDefinition* scriptDefinitionToUnacquire);

        /// Determines whether or not the given definition is currently loaded (currently acquired at least once).
        ///
        /// @param fileName       [in] The file name of the script being loaded, relative to the data directory.
        /// @param makeRelativeTo [in] If 'fileName' is absolute, this will be used to turn it into a relative path.
        ///
        /// @remarks
        ///     All resources must have a relative path somewhere. If it doesn't, there will be errors with serialization. Thus,
        ///     this will return null if 'fileName' is absolute and 'makeRelativeTo' is null.
        static bool IsLoaded(const char* fileName, const char* makeRelativeTo = nullptr);


        /////////////////////////////////////////////////
        // Iteration

        /// Retrieves the number of the loaded definitions. This is used for iteration in conjunction with GetLoadedDefinitionByIndex().
        static size_t GetLoadedDefinitionCount();

        /// Retrieves a pointer to the loaded definition at the given index.
        ///
        /// @param index [in] The index of the definition to retrieve.
        static const ScriptDefinition* GetLoadedDefinitionByIndex(size_t index);
    };
}

#endif