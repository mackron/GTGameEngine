// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef GT_ParticleSystemLibrary
#define GT_ParticleSystemLibrary

#include "ParticleSystem.hpp"

namespace GT
{
    class Context;

    /// Static class for managing loaded particle systems.
    class ParticleSystemLibrary
    {
    public:

        /// Constructor.
        ParticleSystemLibrary(Context &context);

        /// Destructor.
        ~ParticleSystemLibrary();



        /////////////////////////////////////
        // Startup/Shutdown

        /// Starts up the library.
        bool Startup();

        /// Shuts down the library.
        void Shutdown();



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
        ParticleSystem* Create(const char* fileName, const char* makeRelativeTo = nullptr);

        /// Creates a copy of the given particle system.
        ///
        /// @param source [in] A reference to the particle system to copy.
        ///
        /// @remarks
        ///     A pointer to the new particle system, or null if an error occurs.
        ParticleSystem* CreateCopy(const ParticleSystem &source);


        /// Deletes a particle system created with Create() or CreateCopy().
        ///
        /// @param particleSystemToDelete [in] A pointer to the particle system being deleted.
        ///
        /// @remarks
        ///     If the particle system being deleted is the last instantiated of that particle system file, the file will be unloaded.
        void Delete(ParticleSystem* particleSystemToDelete);


        /// Reloads the given particle system. If the particle system has not yet been loaded, it will NOT be loaded.
        ///
        /// @param fileName [in] The name of the file to reload.
        ///
        /// @remarks
        ///     This will update every particle system using the given file.
        bool Reload(const char* fileName);



    private:

        /// A reference to the context that owns this library.
        Context &m_context;




        struct ParticleSystemDefinitionReference
        {
            ParticleSystemDefinition* definition;
            size_t referenceCount;


            ParticleSystemDefinitionReference(ParticleSystemDefinition* definitionIn, size_t referenceCountIn)
                : definition(definitionIn), referenceCount(referenceCountIn)
            {
            }

            ParticleSystemDefinitionReference(const ParticleSystemDefinitionReference &other)
                : definition(other.definition), referenceCount(other.referenceCount)
            {
            }

            ParticleSystemDefinitionReference & operator=(const ParticleSystemDefinitionReference &other)
            {
                this->definition     = other.definition;
                this->referenceCount = other.referenceCount;

                return *this;
            }
        };

        /// The list of loaded particle system definitions, indexed by their absolute path.
        Dictionary<ParticleSystemDefinitionReference> m_loadedDefinitions;

        /// The list of instantiated particle system objects. We need this so we can delete them on shutdown.
        Vector<ParticleSystem*> m_instantiatedParticleSystems;
    };
}

#endif