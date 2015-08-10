// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Assets/Asset.hpp>
#include <GTGameEngine/FileSystem.hpp>

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

    bool Asset::LoadMetadata(const char* absolutePath, FileSystem &fileSystem)
    {
        HFile hMetadataFile = fileSystem.OpenFile(absolutePath, GT::FileAccessMode::Read);
        if (hMetadataFile != 0)
        {
            unsigned int fileSize = static_cast<unsigned int>(fileSystem.GetFileSize(hMetadataFile));
            if (fileSize > 0)
            {
                // Read the data.
                char* fileData = reinterpret_cast<char*>(malloc(fileSize));
                fileSystem.ReadFile(hMetadataFile, fileSize, fileData);
                fileSystem.CloseFile(hMetadataFile);

                GTLib::BasicDeserializer deserializer(fileData, fileSize);
                m_metadata.Deserialize(deserializer);

                free(fileData);
                return true;
            }
            else
            {
                // File size is 0.
                fileSystem.CloseFile(hMetadataFile);
                return false;
            }
        }
        else
        {
            // Failed to open the file.
            return false;
        }
    }
}
