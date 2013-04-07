// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#ifndef __GTEngine_ParticleSystemDefinition_hpp_
#define __GTEngine_ParticleSystemDefinition_hpp_

#include <GTCore/String.hpp>

namespace GTEngine
{
    class ParticleSystemDefinition
    {
    public:

        /// Constructor.
        ParticleSystemDefinition();

        /// Destructor.
        ~ParticleSystemDefinition();


        /// Loads the particle system from a file.
        ///
        /// @remarks
        ///     If 'fileName' is absolute, a relative path must also be specified. It is an error for 'fileName' to be absolute and 'relativePath' to be null.
        bool LoadFromFile(const char* fileName, const char* relativePath = nullptr);


        /// Retrieves the absolute path.
        const char* GetAbsolutePath() const;

        /// Retrieves the relative path.
        const char* GetRelativePath() const;


        



    private:

        /// The absolute paht of the particle system.
        GTCore::String absolutePath;

        /// The relative path of the particle system.
        GTCore::String relativePath;


    };
}

#endif