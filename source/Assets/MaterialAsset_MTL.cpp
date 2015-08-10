// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "MaterialAsset_MTL.hpp"

#if defined(GT_BUILD_MTL)
#include <GTGameEngine/FileSystem.hpp>
#include "../external/easy_mtl/easy_mtl.h"
#include "../external/easy_mtl/extras/easy_mtl_compiler_mtl.h"

namespace GT
{
    MaterialAsset_MTL::MaterialAsset_MTL(AssetType assetType)
        : MaterialAsset(assetType),
          m_pData(nullptr), m_dataSizeInBytes(0)
    {
    }

    MaterialAsset_MTL::~MaterialAsset_MTL()
    {
        free(m_pData);
    }


    bool MaterialAsset_MTL::Load(const char* absolutePath, FileSystem &fileSystem)
    {
        HFile hFile = fileSystem.OpenFile(absolutePath, FileAccessMode::Read);
        if (hFile != 0)
        {
            unsigned int fileSize = static_cast<unsigned int>(fileSystem.GetFileSize(hFile));
            if (fileSize > 6)
            {
                bool result = true;

                char* fileData = reinterpret_cast<char*>(malloc(fileSize + 1));
                fileSystem.ReadFile(hFile, fileSize, fileData);
                fileSystem.CloseFile(hFile);

                fileData[fileSize] = '\0';

                easymtl_material materialSource;
                if (easymtl_compile_wavefront_mtl(&materialSource, fileData, fileSize, "FS_TexCoord"))
                {
                    m_dataSizeInBytes = materialSource.sizeInBytes;
                    m_pData = malloc(materialSource.sizeInBytes);
                    memcpy(m_pData, materialSource.pRawData, materialSource.sizeInBytes);

                    easymtl_uninit(&materialSource);
                    result = true;
                }
                else
                {
                    result = false;
                }


                // At this point the file has been parsed, so free the file data...
                free(fileData);

                return result;
            }
            else
            {
                fileSystem.CloseFile(hFile);
            }
        }

        return false;
    }


    const void* MaterialAsset_MTL::GetData() const
    {
        return m_pData;
    }

    unsigned int MaterialAsset_MTL::GetDataSizeInBytes() const
    {
        return m_dataSizeInBytes;
    }
}

#endif