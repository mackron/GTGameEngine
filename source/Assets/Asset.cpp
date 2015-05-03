// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Assets/Asset.hpp>

namespace GT
{
    Asset::Asset(AssetType type)
        : m_type(type)
    {
    }

    Asset::~Asset()
    {
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
}
