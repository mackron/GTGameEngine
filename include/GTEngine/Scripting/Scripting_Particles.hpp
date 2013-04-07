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


        namespace ParticleFFI
        {
        }

        namespace ParticleEmitterFFI
        {
        }

        namespace ParticleSystemFFI
        {
        }
    }
}

#endif