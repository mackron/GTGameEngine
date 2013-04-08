// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_Scripting_Particles_hpp_
#define __GTEngine_Scripting_Particles_hpp_

#include "../GameScript.hpp"

namespace GTEngine
{
    namespace Scripting
    {
        /// Loads the particles library.
        ///
        /// @param script [in] A reference to the script to load the library into.
        ///
        /// @return True if the library is loaded successfully; false otherwise.
        bool LoadParticlesLibrary(GTCore::Script &script);


        namespace ParticleEmitterFFI
        {
        }

        namespace ParticleSystemFFI
        {
        }

        namespace ParticleSystemDefinitionFFI
        {
            /// Retrieves the number of emitters attached to the given particle system.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle system definition.
            int GetEmitterCount(GTCore::Script &script);

            /// Retrieves a pointer to the emitter at the given index.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle system definition.
            ///     Argument 2: The index of the emitter to retrieve.
            int GetEmitterPtrByIndex(GTCore::Script &script);

            /// Appends a new emitter, returning a pointer to it.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle system definition.
            int AppendNewEmitter(GTCore::Script &script);

            /// Removes an emitter by it's index.
            ///
            /// @remarks
            ///     The emitter will be completely deleted, meaning all pointers to that emitter will be made invalid.
            ///
            ///     Argument 1: A pointer to the particle system definition.
            ///     Argument 2: The index of the emitter to delete.
            int DeleteEmitterByIndex(GTCore::Script &script);
        }
    }
}

#endif