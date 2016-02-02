// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/ParticleSystemDefinition.hpp>
#include <GTGE/GTEngine.hpp>
#include <easy_path/easy_path.h>

namespace GT
{
    ParticleSystemDefinition::ParticleSystemDefinition(Context &context)
        : m_context(context),
          absolutePath(), relativePath(),
          emitters()
    {
    }

    ParticleSystemDefinition::~ParticleSystemDefinition()
    {
        this->Clear();
    }


    bool ParticleSystemDefinition::LoadFromFile(const char* fileNameIn, const char* relativePathIn)
    {
        char newAbsolutePath[DRVFS_MAX_PATH];
        char newRelativePath[DRVFS_MAX_PATH];


        if (easypath_is_absolute(fileNameIn))
        {
            strcpy_s(newAbsolutePath, sizeof(newAbsolutePath), fileNameIn);

            if (relativePathIn != nullptr)
            {
                strcpy_s(newRelativePath, sizeof(newRelativePath), relativePathIn);
            }
            else
            {
                g_Context->LogErrorf("Attempting to load a file using an absolute path (%s). You need to use a path that's relative to the game's data directory.", fileNameIn);
                return false;
            }
        }
        else
        {
            strcpy_s(newRelativePath, sizeof(newRelativePath), fileNameIn);

            if (drvfs_find_absolute_path(g_Context->GetVFS(), fileNameIn, newAbsolutePath, sizeof(newAbsolutePath)))
            {
                return false;
            }
        }


        drvfs_file* pFile = drvfs_open(g_Context->GetVFS(), newAbsolutePath, DRVFS_READ, 0);
        if (pFile != nullptr)
        {
            FileDeserializer deserializer(pFile);
            this->Deserialize(deserializer);


            this->absolutePath = newAbsolutePath;
            this->relativePath = newRelativePath;

            drvfs_close(pFile);
            return true;
        }

        return false;
    }


    const char* ParticleSystemDefinition::GetAbsolutePath() const
    {
        return this->absolutePath.c_str();
    }

    const char* ParticleSystemDefinition::GetRelativePath() const
    {
        return this->relativePath.c_str();
    }




    size_t ParticleSystemDefinition::GetEmitterCount() const
    {
        return this->emitters.count;
    }


    ParticleEmitter* ParticleSystemDefinition::GetEmitter(size_t index)
    {
        return this->emitters[index];
    }

    const ParticleEmitter* ParticleSystemDefinition::GetEmitter(size_t index) const
    {
        return this->emitters[index];
    }


    ParticleEmitter* ParticleSystemDefinition::AppendNewEmitter()
    {
        auto newEmitter = new ParticleEmitter(m_context);
        this->emitters.PushBack(newEmitter);

        return newEmitter;
    }

    void ParticleSystemDefinition::DeleteEmitterByIndex(size_t index)
    {
        delete this->emitters[index];
        this->emitters.Remove(index);
    }




    //////////////////////////////////////
    // Serialization/Deserialization

    void ParticleSystemDefinition::Serialize(Serializer &serializer)
    {
        BasicSerializer intermediarySerializer;

        uint32_t emitterCount = static_cast<uint32_t>(this->emitters.count);
        intermediarySerializer.Write(emitterCount);


        // Emitters.
        for (uint32_t iEmitter = 0; iEmitter < emitterCount; ++iEmitter)
        {
            auto emitter = this->emitters[iEmitter];
            assert(emitter != nullptr);
            {
                emitter->Serialize(intermediarySerializer, false);      // <-- 'false' means that individual particle information will not be serialized (not needed for definitions).
            }
        }



        Serialization::ChunkHeader header;
        header.id          = Serialization::ChunkID_ParticleSystemDefinition_Main;
        header.version     = 1;
        header.sizeInBytes = intermediarySerializer.GetBufferSizeInBytes();
        
        serializer.Write(header);
        serializer.Write(intermediarySerializer.GetBuffer(), header.sizeInBytes);
    }

    void ParticleSystemDefinition::Deserialize(Deserializer &deserializer)
    {
        Serialization::ChunkHeader header;
        deserializer.Read(header);

        if (header.id == Serialization::ChunkID_ParticleSystemDefinition_Main)
        {
            // We'll first want to clear the definition (reset it).
            this->Clear();

            uint32_t emitterCount;
            deserializer.Read(emitterCount);

            for (size_t iEmitter = 0; iEmitter < emitterCount; ++iEmitter)
            {
                auto emitter = this->AppendNewEmitter();
                assert(emitter != nullptr);
                {
                    emitter->Deserialize(deserializer);
                }
            }
        }
        else
        {
            g_Context->LogErrorf("Error deserializing particle system definition. Unknown chunk ID (%d).", header.id);
        }
    }



    //////////////////////////////////////
    // Private

    void ParticleSystemDefinition::Clear()
    {
        for (size_t i = 0; i < this->emitters.count; ++i)
        {
            delete this->emitters[i];
        }
        this->emitters.Clear();
    }
}