// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/ParticleSystemLibrary.hpp>
#include <GTGE/GTEngine.hpp>
#include <GTGE/Core/Dictionary.hpp>
#include <GTGE/Core/Vector.hpp>
#include <dr_libs/dr_path.h>


namespace GT
{
    ParticleSystemLibrary::ParticleSystemLibrary(Context &context)
        : m_context(context), m_loadedDefinitions()
    {
    }

    ParticleSystemLibrary::~ParticleSystemLibrary()
    {
    }



    /////////////////////////////////////
    // Startup/Shutdown

    bool ParticleSystemLibrary::Startup()
    {
        return true;
    }

    void ParticleSystemLibrary::Shutdown()
    {
        // Instantiated particle systems need to be deleted.
        for (size_t i = 0; i < m_instantiatedParticleSystems.count; ++i)
        {
            delete m_instantiatedParticleSystems[i];
        }
        m_instantiatedParticleSystems.Clear();


        // Definitions now need to be deleted.
        for (size_t i = 0; i < m_loadedDefinitions.count; ++i)
        {
            delete m_loadedDefinitions.buffer[i]->value.definition;
        }
        m_loadedDefinitions.Clear();
    }



    /////////////////////////////////////
    // Creation/Deletion

    ParticleSystem* ParticleSystemLibrary::Create(const char* fileName, const char* makeRelativeTo)
    {
        char relativePath[DRFS_MAX_PATH];
        strcpy_s(relativePath, sizeof(relativePath), fileName);

        if (drpath_is_absolute(fileName))
        {
            if (makeRelativeTo != nullptr)
            {
                drpath_to_relative(fileName, makeRelativeTo, relativePath, sizeof(relativePath));
            }
            else
            {
                g_Context->LogErrorf("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileName);
                return nullptr;
            }
        }


        char absolutePath[DRFS_MAX_PATH];
        if (drfs_find_absolute_path(g_Context->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            ParticleSystemDefinition* definition = nullptr;

            auto iDefinition = m_loadedDefinitions.Find(absolutePath);
            if (iDefinition != nullptr)
            {
                // Definition is already loaded. All we do it increment the reference counter.
                iDefinition->value.referenceCount += 1;

                definition = iDefinition->value.definition;
            }
            else
            {
                // Definition is not yet loaded.
                definition = new ParticleSystemDefinition(m_context);
                if (definition->LoadFromFile(absolutePath, relativePath))
                {
                    m_loadedDefinitions.Add(absolutePath, ParticleSystemDefinitionReference(definition, 1));
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
                m_instantiatedParticleSystems.PushBack(particleSystem);

                return particleSystem;
            }
        }

        return nullptr;
    }

    ParticleSystem* ParticleSystemLibrary::CreateCopy(const ParticleSystem &other)
    {
        auto newParticleSystem = new ParticleSystem(other.GetDefinition());
        m_instantiatedParticleSystems.PushBack(newParticleSystem);


        // The reference counter needs to be incremented.
        auto iDefinition = m_loadedDefinitions.Find(other.GetDefinition().GetAbsolutePath());
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
            m_instantiatedParticleSystems.RemoveFirstOccuranceOf(particleSystemToDelete);


            // The reference counter needs to be decremented. If this is the last reference to the particle system we'll delete it.
            String absolutePath(particleSystemToDelete->GetDefinition().GetAbsolutePath());

            auto iDefinition = m_loadedDefinitions.Find(absolutePath.c_str());
            if (iDefinition != nullptr)
            {
                assert(iDefinition->value.referenceCount >= 1);
                {
                    iDefinition->value.referenceCount -= 1;

                    if (iDefinition->value.referenceCount == 0)
                    {
                        delete iDefinition->value.definition;
                        m_loadedDefinitions.RemoveByKey(absolutePath.c_str());
                    }
                }
            }


            delete particleSystemToDelete;
        }
    }

    bool ParticleSystemLibrary::Reload(const char* fileName)
    {
        char absolutePath[DRFS_MAX_PATH];
        if (drfs_find_absolute_path(g_Context->GetVFS(), fileName, absolutePath, sizeof(absolutePath)))
        {
            auto iDefinition = m_loadedDefinitions.Find(absolutePath);
            if (iDefinition != nullptr)
            {
                auto &reference = iDefinition->value;
                assert(reference.definition != nullptr);
                {
                    bool result = reference.definition->LoadFromFile(reference.definition->GetAbsolutePath(), reference.definition->GetRelativePath());
                    if (result)
                    {
                        for (size_t iParticleSystem = 0; iParticleSystem < m_instantiatedParticleSystems.count; ++iParticleSystem)
                        {
                            auto particleSystem = m_instantiatedParticleSystems.buffer[iParticleSystem];
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