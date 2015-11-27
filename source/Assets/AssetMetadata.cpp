// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Assets/AssetMetadata.hpp>

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4351)   // new behaviour
#endif

namespace GT
{
    //////////////////////////////////////
    // AssetMetadataChunk

    AssetMetadataChunk::AssetMetadataChunk(const char* name, uint32_t dataSizeInBytes, const void* pData)
        : m_pData(nullptr), m_name(), m_dataSizeInBytes(dataSizeInBytes)
    {
        // Name.
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<uint64_t*>(m_name)[i] = 0;
        }

        for (int i = 0; i < 31 && name[i] != '\0'; ++i)
        {
            m_name[i] = name[i];
        }


        // Data.
        m_pData = malloc(dataSizeInBytes);
        memcpy(m_pData, pData, dataSizeInBytes);
    }

    AssetMetadataChunk::AssetMetadataChunk(GTLib::Deserializer &deserializer)
        : m_pData(nullptr), m_name(), m_dataSizeInBytes(0)
    {
        // Name.
        deserializer.Read(m_name, 32);

        // Data size.
        deserializer.Read(m_dataSizeInBytes);

        // Data.
        m_pData = malloc(m_dataSizeInBytes);
        deserializer.Read(m_pData, m_dataSizeInBytes);
    }

    AssetMetadataChunk::~AssetMetadataChunk()
    {
        free(m_pData);
    }


    void AssetMetadataChunk::SetName(const char* name)
    {
        for (int i = 0; i < 4; ++i)
        {
            reinterpret_cast<uint64_t*>(m_name)[i] = 0;
        }

        for (int i = 0; i < 31 && name[i] != '\0'; ++i)
        {
            m_name[i] = name[i];
        }
    }

    const char* AssetMetadataChunk::GetName() const
    {
        return m_name;
    }


    void AssetMetadataChunk::SetData(uint32_t sizeInBytes, const void* pData)
    {
        // Free the old data.
        free(m_pData);

        // Copy over the new data.
        m_dataSizeInBytes = sizeInBytes;
        memcpy(m_pData, pData, sizeInBytes);
    }

    uint32_t AssetMetadataChunk::GetDataSizeInBytes() const
    {
        return m_dataSizeInBytes;
    }

    const void* AssetMetadataChunk::GetData() const
    {
        return m_pData;
    }


    void AssetMetadataChunk::Serialize(GTLib::Serializer &serializer) const
    {
        serializer.Write(m_name, 32);
        serializer.Write(m_dataSizeInBytes);
        serializer.Write(m_pData, m_dataSizeInBytes);
    }



    //////////////////////////////////////
    // AssetMetadata

    AssetMetadata::AssetMetadata()
        : m_chunks()
    {
    }

    AssetMetadata::AssetMetadata(GTLib::Deserializer &deserializer)
        : m_chunks()
    {
        uint32_t chunkCount;
        deserializer.Read(chunkCount);

        for (uint32_t iChunk = 0; iChunk < chunkCount; ++iChunk)
        {
            m_chunks.PushBack(new AssetMetadataChunk(deserializer));
        }
    }

    AssetMetadata::~AssetMetadata()
    {
        for (size_t i = 0; i < m_chunks.GetCount(); ++i)
        {
            delete m_chunks[i];
        }
    }


    void AssetMetadata::AddChunk(const char* name, uint32_t sizeInBytes, const void* pData)
    {
        // Create the new chunk.
        AssetMetadataChunk* pNewChunk = new AssetMetadataChunk(name, sizeInBytes, pData);


        // Check if a chunk of the same name already exists.
        for (size_t iChunk = 0; iChunk < m_chunks.GetCount(); ++iChunk)
        {
            auto pChunk = m_chunks[iChunk];
            assert(pChunk != nullptr);
            {
                if (strcmp(name, m_chunks[iChunk]->GetName()) == 0)
                {
                    // A chunk of the same name exists - replace it.
                    delete m_chunks[iChunk];
                    m_chunks[iChunk] = pNewChunk;

                    return;
                }
            }
        }

        // If we get here it means a chunk of the same name does not exist. In this case we just push it to the back.
        m_chunks.PushBack(pNewChunk);
    }

    void AssetMetadata::RemoveChunk(const char* name)
    {
        for (size_t iChunk = 0; iChunk < m_chunks.GetCount(); ++iChunk)
        {
            auto pChunk = m_chunks[iChunk];
            assert(pChunk != nullptr);
            {
                if (strcmp(name, pChunk->GetName()) == 0)
                {
                    // A chunk of the same name exists - replace it.
                    delete pChunk;
                    m_chunks.Remove(iChunk);

                    return;
                }
            }
        }
    }

    const void* AssetMetadata::GetChunkData(const char* name, uint32_t &sizeInBytesOut) const
    {
        for (size_t iChunk = 0; iChunk < m_chunks.GetCount(); ++iChunk)
        {
            auto pChunk = m_chunks[iChunk];
            assert(pChunk != nullptr);
            {
                if (strcmp(name, pChunk->GetName()) == 0)
                {
                    // We found the chunk.
                    sizeInBytesOut = pChunk->GetDataSizeInBytes();
                    return pChunk->GetData();
                }
            }
        }

        // The chunk doesn't exist.
        return nullptr;
    }


    void AssetMetadata::Clear()
    {
        for (size_t i = 0; i < m_chunks.GetCount(); ++i)
        {
            delete m_chunks[i];
        }

        m_chunks.Clear();
    }


    void AssetMetadata::Serialize(GTLib::Serializer &serializer) const
    {
        serializer.Write(static_cast<uint32_t>(m_chunks.GetCount()));

        for (uint32_t iChunk = 0; iChunk < static_cast<uint32_t>(m_chunks.GetCount()); ++iChunk)
        {
            auto pChunk = m_chunks[iChunk];
            assert(pChunk != nullptr);
            {
                pChunk->Serialize(serializer);
            }
        }
    }

    void AssetMetadata::Deserialize(GTLib::Deserializer &deserializer)
    {
        // Clear.
        this->Clear();


        // Read the data.
        uint32_t chunkCount;
        deserializer.Read(chunkCount);

        for (uint32_t iChunk = 0; iChunk < chunkCount; ++iChunk)
        {
            m_chunks.PushBack(new AssetMetadataChunk(deserializer));
        }
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
