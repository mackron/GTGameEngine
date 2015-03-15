// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTGameEngine/Config.hpp>

#if defined(GT_BUILD_DEFAULT_ASSETS)
#include "DefaultAssetAllocator.hpp"
#include "ImageAsset_STB.hpp"

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

        //return (type > 0x00000000 && type < 0x0000FFFF);
    }

    Asset* DefaultAssetAllocator::CreateAsset(AssetType type)
    {
        // Images
        if (type >= AssetType_Image_PNG && type <= AssetType_Image_PSD)
        {
            return new ImageAsset_STB(type);
        }


        // Models
        if (type == AssetType_Model_OBJ)
        {
            //return new ModelAsset_OBJ(type);
        }

        if (type == AssetType_Model_OGEX)
        {
            //return new ModelAsset_OGEX(type);
        }


        // Sounds
        if (type == AssetType_Sound_WAV)
        {
            //return new SoundAsset_WAV(type);
        }

        if (type == AssetType_Sound_OGG)
        {
            //return new SoundAsset_OGG(type);
        }



        return nullptr;
    }

    void DefaultAssetAllocator::DeleteAsset(Asset* pAsset)
    {
        delete pAsset;
    }
}

#endif
