// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include "MaterialAsset_MTL.hpp"

#if defined(GT_BUILD_MTL)
#include <easy_draw/easy_mtl.h>

namespace GT
{
    MaterialAsset_MTL::MaterialAsset_MTL(const char* absolutePathOrIdentifier, AssetType assetType)
        : MaterialAsset(absolutePathOrIdentifier, assetType),
          m_pData(nullptr), m_dataSizeInBytes(0)
    {
    }

    MaterialAsset_MTL::~MaterialAsset_MTL()
    {
        free(m_pData);
    }


    bool MaterialAsset_MTL::Load(const char* absolutePath, drvfs_context* pVFS)
    {
        size_t fileSize;
        char* pFileData = drvfs_open_and_read_text_file(pVFS, absolutePath, &fileSize);
        if (pFileData != NULL && fileSize > 6)
        {
            bool result = true;

            easymtl_material materialSource;
            if (easymtl_compile_wavefront_mtl(&materialSource, pFileData, fileSize, "FS_TexCoord"))
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


            drvfs_free(pFileData);
            return result;
        }
        else
        {
            return false;
        }
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
