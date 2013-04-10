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
            /// Enables burst mode.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            int EnableBurstMode(GTCore::Script &script);

            /// Disables burst mode.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            int DisableBurstMode(GTCore::Script &script);

            /// Determines whether or not burst mode is enabled.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            int IsBurstModeEnabled(GTCore::Script &script);


            /// Sets the duration in seconds.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            ///     Argument 2: The duration in seconds.
            int SetDurationInSeconds(GTCore::Script &script);

            /// Retrieves the duration in seconds.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            int GetDurationInSeconds(GTCore::Script &script);


            /// Sets the emission rate per second.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            ///     Argument 2: The emission rate per second.
            int SetEmissionRatePerSecond(GTCore::Script &script);

            /// Retrieves the emission rate per second.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            int GetEmissionRatePerSecond(GTCore::Script &script);


            /// Sets the start speed of the particles of the emitter.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            ///     Argument 2: The new start speed.
            int SetStartSpeed(GTCore::Script &script);

            /// Retrieves the start speed of the particles of the emitter.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            int GetStartSpeed(GTCore::Script &script);


            /// Sets the lifetime of the particles of the emitter.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            ///     Argument 2: The new lifetime.
            int SetLifetime(GTCore::Script &script);

            /// Retrieves the lifetime of the particles of the emitter.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            int GetLifetime(GTCore::Script &script);


            /// Sets the gravity factor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            ///     Argument 2: The new gravity factor.
            int SetGravityFactor(GTCore::Script &script);

            /// Retrieves the gravity factor.
            ///
            /// @remarks
            ///     Argument 1: A pointer to the particle emitter.
            int GetGravityFactor(GTCore::Script &script);
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