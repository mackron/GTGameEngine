// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGE/Assets/Asset.hpp>
#include <easy_util/easy_util.h>

namespace GT
{
    Asset::Asset(const char* absolutePathOrIdentifier, AssetType type)
        : m_absolutePathOrIdentifier(), m_type(type), m_metadata()
    {
        strcpy_s(m_absolutePathOrIdentifier, DRVFS_MAX_PATH, absolutePathOrIdentifier);
    }

    Asset::~Asset()
    {
    }


    const char * Asset::GetAbsolutePathOrIdentifier() const
    {
        return m_absolutePathOrIdentifier;
    }

    AssetType Asset::GetType() const
    {
        return m_type;
    }


    void Asset::AddMetadataChunk(const char* name, uint32_t sizeInBytes, const void* pData)
    {
        m_metadata.AddChunk(name, sizeInBytes, pData);
    }

    const void* Asset::GetMetadataChunkData(const char* name, uint32_t &sizeInBytesOut) const
    {
        return m_metadata.GetChunkData(name, sizeInBytesOut);
    }

    const void* Asset::GetMetadataChunkData(const char* name) const
    {
        uint32_t unused;
        return this->GetMetadataChunkData(name, unused);
    }

    bool Asset::LoadMetadata(const char* absolutePath, drvfs_context* pVFS)
    {
        drvfs_file* pMetadataFile = drvfs_open(pVFS, absolutePath, DRVFS_READ, 0);
        if (pMetadataFile != nullptr)
        {
            FileDeserializer deserializer(pMetadataFile);
            m_metadata.Deserialize(deserializer);

            drvfs_close(pMetadataFile);
            return true;
        }
        else
        {
            // Failed to open the file.
            return false;
        }
    }
}
