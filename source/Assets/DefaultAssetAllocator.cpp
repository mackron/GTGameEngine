// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#include <GTEngine/Config.hpp>
#include <easy_path/easy_path.h>
#include <easy_util/easy_util.h>

#if defined(GT_BUILD_DEFAULT_ASSETS)
#include "DefaultAssetAllocator.hpp"
#include "ImageAsset_STB.hpp"

#if defined(GT_BUILD_MTL)
#include "MaterialAsset_MTL.hpp"
#endif

#if defined(GT_BUILD_MD2)
#include "ModelAsset_MD2.hpp"
#endif

#if defined(GT_BUILD_OBJ)
#include "ModelAsset_OBJ.hpp"
#endif

#if defined(GT_BUILD_WAV)
#include "SoundAsset_WAV.hpp"
#endif

#if defined(GT_BUILD_VORBIS)
#include "SoundAsset_Vorbis.hpp"
#endif

namespace GT
{
    AssetType DefaultAssetAllocator::GetAssetTypeByPath(const char* filePath) const
    {
        // We determine the type by the extension, except for Wavefront MTL files which need to the in the format of <file>.mtl/<material>
        const char* ext = easypath_extension(filePath);
        if (ext != NULL)
        {
            if (ext[0] != '\0')
            {
                if (Strings::Equal<false>(ext, "png"))
                {
                    return AssetType_Image_PNG;
                }
                if (Strings::Equal<false>(ext, "tga"))
                {
                    return AssetType_Image_TGA;
                }
                if (Strings::Equal<false>(ext, "jpg") || Strings::Equal<false>(ext, "jpeg"))
                {
                    return AssetType_Image_JPG;
                }
                if (Strings::Equal<false>(ext, "psd"))
                {
                    return AssetType_Image_PSD;
                }

                if (Strings::Equal<false>(ext, "obj"))
                {
                    return AssetType_Model_OBJ;
                }
                if (Strings::Equal<false>(ext, "md2"))
                {
                    return AssetType_Model_MD2;
                }
                if (Strings::Equal<false>(ext, "ogex"))
                {
                    return AssetType_Model_OGEX;
                }

                if (Strings::Equal<false>(ext, "wav"))
                {
                    return AssetType_Sound_WAV;
                }
                if (Strings::Equal<false>(ext, "ogg"))
                {
                    return AssetType_Sound_Vorbis;
                }
            }
            else
            {
                // There's no extension, so it could be a material in a Wavefront MTL file. To do this we look for the last dot.
                const char* filename = easypath_filename(filePath);
                if (filename != NULL && filename[0] != '\0')
                {
                    const char* extension = filename;
                    while (extension >= filePath && extension[0] != '.')
                    {
                        extension -= 1;
                    }

                    if (extension[0] == '.')
                    {
                        extension += 1;

                        if ((filename - extension) > 3)
                        {
                            if ((extension[0] == 'm' || extension[0] == 'M') && (extension[1] == 't' || extension[1] == 'T') && (extension[2] == 'l' || extension[2] == 'L'))
                            {
                                if (extension[3] == '/')
                                {
                                    // It's a Wavefront material.
                                    return AssetType_Material_MTL;
                                }
                            }
                        }
                    }
                }
            }
        }

        return AssetType_Unknown;
    }

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

#if defined(GT_BUILD_MTL)
        case AssetType_Material_MTL:
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
#if defined(GT_BUILD_VORBIS)
        case AssetType_Sound_Vorbis:
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

    Asset* DefaultAssetAllocator::CreateAsset(const char* absolutePathOrIdentifier, AssetType type)
    {
        // Images
        if (type >= AssetType_Image_PNG && type <= AssetType_Image_PSD)
        {
            return new ImageAsset_STB(absolutePathOrIdentifier, type);
        }


        // Materials
#if defined(GT_BUILD_MTL)
        if (type == AssetType_Material_MTL)
        {
            return new MaterialAsset_MTL(absolutePathOrIdentifier, type);
        }
#endif


        // Models
#if defined(GT_BUILD_OBJ)
        if (type == AssetType_Model_OBJ)
        {
            return new ModelAsset_OBJ(absolutePathOrIdentifier, type);
        }
#endif

#if defined(GT_BUILD_MD2)
        if (type == AssetType_Model_MD2)
        {
            return new ModelAsset_MD2(absolutePathOrIdentifier, type);
        }
#endif

#if defined(GT_BUILD_OGEX)
        if (type == AssetType_Model_OGEX)
        {
            //return new ModelAsset_OGEX(absolutePathOrIdentifier, type);
        }
#endif


        // Sounds
#if defined(GT_BUILD_WAV)
        if (type == AssetType_Sound_WAV)
        {
            return new SoundAsset_WAV(absolutePathOrIdentifier, type);
        }
#endif

#if defined(GT_BUILD_VORBIS)
        if (type == AssetType_Sound_Vorbis)
        {
            return new SoundAsset_Vorbis(absolutePathOrIdentifier, type);
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
