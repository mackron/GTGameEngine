// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ParticleSystemLibrary_hpp_
#define __GTEngine_ParticleSystemLibrary_hpp_

#include "ParticleSystem.hpp"

namespace GTEngine
{
    /// Static class for managing loaded particle systems.
    class ParticleSystemLibrary
    {
    public:

        /////////////////////////////////////
        // Startup/Shutdown

        /// Starts up the library.
        static bool Startup();

        /// Shuts down the library.
        static void Shutdown();



        /////////////////////////////////////
        // Creation/Deletion

        /// Creates a new particle system from a file.
        ///
        /// @param fileName       [in] The name of the particle system file to load.
        /// @param makeRelativeTo [in] If 'fileName' is absolute, it will be made relative to this path.
        ///
        /// @return A pointer to the new particle system.
        ///
        /// @remarks
        ///     Delete the returned particle system with Delete(). The library does not do memory management - it must be deleted manually.
        ///
        ///     This will not reload the file if it has previously been loaded. Thus, calling this function with the same file will be quick
        ///     after the first load. Use Reload() to reload a file.
        static ParticleSystem* Create(const char* fileName, const char* makeRelativeTo = nullptr);

        /// Creates a copy of the given particle system.
        ///
        /// @param source [in] A reference to the particle system to copy.
        ///
        /// @remarks
        ///     A pointer to the new particle system, or null if an error occurs.
        static ParticleSystem* CreateCopy(const ParticleSystem &source);


        /// Deletes a particle system created with Create() or CreateCopy().
        ///
        /// @param particleSystemToDelete [in] A pointer to the particle system being deleted.
        ///
        /// @remarks
        ///     If the particle system being deleted is the last instantiated of that particle system file, the file will be unloaded.
        static void Delete(ParticleSystem* particleSystemToDelete);


        /// Reloads the given particle system. If the particle system has not yet been loaded, it will NOT be loaded.
        ///
        /// @param fileName [in] The name of the file to reload.
        ///
        /// @remarks
        ///     This will update every particle system using the given file.
        static bool Reload(const char* fileName);
    };
}

#endif