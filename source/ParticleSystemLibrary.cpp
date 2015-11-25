// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/ParticleSystemLibrary.hpp>
#include <GTEngine/Errors.hpp>
#include <GTEngine/GTEngine.hpp>
#include <GTLib/Dictionary.hpp>
#include <GTLib/Vector.hpp>
#include <GTLib/Path.hpp>
#include <easy_path/easy_path.h>


namespace GTEngine
{
    /////////////////////////////////////
    // Globals

    struct ParticleSystemDefinitionReference
    {
        ParticleSystemDefinition* definition;
        size_t                    referenceCount;


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
    static GTLib::Dictionary<ParticleSystemDefinitionReference> LoadedDefinitions;

    /// The list of instantiated particle system objects. We need this so we can delete them on shutdown.
    static GTLib::Vector<ParticleSystem*> InstantiatedParticleSystems;



    /////////////////////////////////////
    // Startup/Shutdown

    bool ParticleSystemLibrary::Startup()
    {
        return true;
    }

    void ParticleSystemLibrary::Shutdown()
    {
        // Instantiated particle systems need to be deleted.
        for (size_t i = 0; i < InstantiatedParticleSystems.count; ++i)
        {
            delete InstantiatedParticleSystems[i];
        }
        InstantiatedParticleSystems.Clear();


        // Definitions now need to be deleted.
        for (size_t i = 0; i < LoadedDefinitions.count; ++i)
        {
            delete LoadedDefinitions.buffer[i]->value.definition;
        }
        LoadedDefinitions.Clear();
    }



    /////////////////////////////////////
    // Creation/Deletion

    ParticleSystem* ParticleSystemLibrary::Create(const char* fileName, const char* makeRelativeTo)
    {
        char relativePath[EASYVFS_MAX_PATH];
        strcpy_s(relativePath, sizeof(relativePath), fileName);

        if (GTLib::Path::IsAbsolute(fileName))
        {
            if (makeRelativeTo != nullptr)
            {
                easypath_to_relative(fileName, makeRelativeTo, relativePath, sizeof(relativePath));
            }
            else
            {
                GTEngine::PostError("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return nullptr;
            }
        }


        char absolutePath[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(g_EngineContext->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            ParticleSystemDefinition* definition = nullptr;

            auto iDefinition = LoadedDefinitions.Find(absolutePath);
            if (iDefinition != nullptr)
            {
                // Definition is already loaded. All we do it increment the reference counter.
                iDefinition->value.referenceCount += 1;

                definition = iDefinition->value.definition;
            }
            else
            {
                // Definition is not yet loaded.
                definition = new ParticleSystemDefinition;
                if (definition->LoadFromFile(absolutePath, relativePath))
                {
                    LoadedDefinitions.Add(absolutePath, ParticleSystemDefinitionReference(definition, 1));
                }
                else
                {
                    delete definition;
                    definition = nullptr;
                }
            }

            // With the definition retrieved, we now need to move on to create the actual particle system.
            if (definition != nullptr)
            {
                auto particleSystem = new ParticleSystem(*definition);
                InstantiatedParticleSystems.PushBack(particleSystem);

                return particleSystem;
            }
        }

        return nullptr;
    }

    ParticleSystem* ParticleSystemLibrary::CreateCopy(const ParticleSystem &other)
    {
        auto newParticleSystem = new ParticleSystem(other.GetDefinition());
        InstantiatedParticleSystems.PushBack(newParticleSystem);


        // The reference counter needs to be incremented.
        auto iDefinition = LoadedDefinitions.Find(other.GetDefinition().GetAbsolutePath());
        if (iDefinition != nullptr)
        {
            iDefinition->value.referenceCount += 1;
        }


        return newParticleSystem;
    }

    void ParticleSystemLibrary::Delete(ParticleSystem* particleSystemToDelete)
    {
        if (particleSystemToDelete != nullptr)
        {
            InstantiatedParticleSystems.RemoveFirstOccuranceOf(particleSystemToDelete);


            // The reference counter needs to be decremented. If this is the last reference to the particle system we'll delete it.
            GTLib::String absolutePath(particleSystemToDelete->GetDefinition().GetAbsolutePath());

            auto iDefinition = LoadedDefinitions.Find(absolutePath.c_str());
            if (iDefinition != nullptr)
            {
                assert(iDefinition->value.referenceCount >= 1);
                {
                    iDefinition->value.referenceCount -= 1;

                    if (iDefinition->value.referenceCount == 0)
                    {
                        delete iDefinition->value.definition;
                        LoadedDefinitions.RemoveByKey(absolutePath.c_str());
                    }
                }
            }


            delete particleSystemToDelete;
        }
    }

    bool ParticleSystemLibrary::Reload(const char* fileName)
    {
        char absolutePath[EASYVFS_MAX_PATH];
        if (easyvfs_find_absolute_path(g_EngineContext->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            auto iDefinition = LoadedDefinitions.Find(absolutePath);
            if (iDefinition != nullptr)
            {
                auto &reference = iDefinition->value;
                assert(reference.definition != nullptr);
                {
                    bool result = reference.definition->LoadFromFile(reference.definition->GetAbsolutePath(), reference.definition->GetRelativePath());
                    if (result)
                    {
                        for (size_t iParticleSystem = 0; iParticleSystem < InstantiatedParticleSystems.count; ++iParticleSystem)
                        {
                            auto particleSystem = InstantiatedParticleSystems.buffer[iParticleSystem];
                            assert(particleSystem != nullptr);
                            {
                                if (&particleSystem->GetDefinition() == reference.definition)
                                {
                                    particleSystem->Refresh();
                                }
                            }
                        }

                        return true;
                    }
                }
            }
        }

        return false;
    }
}