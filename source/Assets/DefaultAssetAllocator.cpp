// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Config.hpp>

#if defined(GT_BUILD_DEFAULT_ASSETS)
#include "DefaultAssetAllocator.hpp"
#include "ImageAsset_STB.hpp"

#if defined(GT_BUILD_MD2)
#include "ModelAsset_MD2.hpp"
#endif

#if defined(GT_BUILD_OBJ)
#include "ModelAsset_OBJ.hpp"
#endif

namespace GT
{
    bool DefaultAssetAllocator::IsAssetTypeSupported(AssetType type) const
    {
        switch (type)
        {
#if defined(GT_BUILD_PNG)
        case AssetType_Image_PNG:
#endif
#if defined(GT_BUILD_TGA)
        case AssetType_Image_TGA:
#endif
#if defined(GT_BUILD_JPG)
        case AssetType_Image_JPG:
#endif
#if defined(GT_BUILD_PSD)
        case AssetType_Image_PSD:
#endif

#if defined(GT_BUILD_OBJ)
        case AssetType_Model_OBJ:
#endif
#if defined(GT_BUILD_MD2)
        case AssetType_Model_MD2:
#endif
#if defined(GT_BUILD_OGEX)
        case AssetType_Model_OGEX:
#endif

#if defined(GT_BUILD_WAV)
        case AssetType_Sound_WAV:
#endif
#if defined(GT_BUILD_OGG)
        case AssetType_Sound_OGG:
#endif
            {
                return true;
            }

        default:
            {
                return false;
            }
        }
    }

    Asset* DefaultAssetAllocator::CreateAsset(AssetType type)
    {
        // Images
        if (type >= AssetType_Image_PNG && type <= AssetType_Image_PSD)
        {
            return new ImageAsset_STB(type);
        }


        // Models
#if defined(GT_BUILD_OBJ)
        if (type == AssetType_Model_OBJ)
        {
            return new ModelAsset_OBJ(type);
        }
#endif

#if defined(GT_BUILD_MD2)
        if (type == AssetType_Model_MD2)
        {
            return new ModelAsset_MD2(type);
        }
#endif

#if defined(GT_BUILD_OGEX)
        if (type == AssetType_Model_OGEX)
        {
            //return new ModelAsset_OGEX(type);
        }
#endif


        // Sounds
#if defined(GT_BUILD_WAV)
        if (type == AssetType_Sound_WAV)
        {
            //return new SoundAsset_WAV(type);
        }
#endif

#if defined(GT_BUILD_OGG)
        if (type == AssetType_Sound_OGG)
        {
            //return new SoundAsset_OGG(type);
        }
#endif



        return nullptr;
    }

    void DefaultAssetAllocator::DeleteAsset(Asset* pAsset)
    {
        delete pAsset;
    }
}

#endif
