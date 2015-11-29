// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTEngine/IO.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTLib/ImageLoader.hpp>
#include <GTLib/Path.hpp>
#include <GTLib/Strings/Find.hpp>
#include <easy_path/easy_path.h>

namespace GT
{
    bool IsSupportedImageExtension(const char* fileName)
    {
        return GTLib::ImageLoader::IsExtensionSupported(GTLib::Path::Extension(fileName));
    }

    bool IsSupportedModelExtension(const char* fileName)
    {
        return GTEngine::ModelLibrary::IsExtensionSupported(GTLib::Path::Extension(fileName));
    }

    bool IsSupportedMaterialExtension(const char* fileName)
    {
        auto extension = GTLib::Path::Extension(fileName);

        return GTLib::Strings::Equal<false>(extension, "material") ||
                GTLib::Strings::Equal<false>(extension, "gtmaterial");
    }

    bool IsSupportedSoundExtension(const char* fileName)
    {
        const char* extension = GTLib::Path::Extension(fileName);
            
        return GTLib::Strings::Equal<false>(extension, "wav") ||
                GTLib::Strings::Equal<false>(extension, "ogg");
    }

    bool IsSupportedParticleSystemExtension(const char* fileName)
    {
        return GTLib::Strings::Equal<false>(GTLib::Path::Extension(fileName), "gtparticle");
    }

    bool IsSupportedSceneExtension(const char* fileName)
    {
        return GTLib::Strings::Equal<false>(GTLib::Path::Extension(fileName), "gtscene");
    }

    bool IsSupportedPrefabExtension(const char* fileName)
    {
        return GTLib::Strings::Equal<false>(GTLib::Path::Extension(fileName), "gtprefab");
    }

    bool IsSupportedScriptExtension(const char* fileName)
    {
        auto extension = GTLib::Path::Extension(fileName);

        return GTLib::Strings::Equal<false>(extension, "gtscript") ||
                GTLib::Strings::Equal<false>(extension, "script")   ||
                GTLib::Strings::Equal<false>(extension, "lua");
    }



    AssetType GetAssetClassFromExtension(const char* fileName)
    {
        // If the path is a directory instead of a file, we need to return AssetType_None.
        if (GTLib::Strings::IsNullOrEmpty(easypath_filename(fileName)))
        {
            return AssetClass_Unknown;
        }
        else
        {
            if (IsSupportedImageExtension(fileName))
            {
                return AssetClass_Image;
            }
            else if (IsSupportedModelExtension(fileName))
            {
                return AssetClass_Model;
            }
            else if (IsSupportedMaterialExtension(fileName))
            {
                return AssetClass_Material;
            }
            else if (IsSupportedSoundExtension(fileName))
            {
                return AssetClass_Sound;
            }
            else if (IsSupportedParticleSystemExtension(fileName))
            {
                return AssetClass_ParticleSystem;
            }
            else if (IsSupportedSceneExtension(fileName))
            {
                return AssetClass_Scene;
            }
            else if (IsSupportedPrefabExtension(fileName))
            {
                return AssetClass_Prefab;
            }
            else if (IsSupportedScriptExtension(fileName))
            {
                return AssetClass_Script;
            }
            else
            {
                // If we don't have an extension assume it's a text file.

                return AssetClass_TextFile;
            }
        }
    }



    GTLib::String GetBasePath(const char* absolutePath, const char* relativePath)
    {
        // All we're going to do is remove the 'relativePath' part from the absolute path.
        const char* start = absolutePath;
        const char* end   = GTLib::Strings::FindFirst(absolutePath, relativePath);

        return GTLib::String(start, end - start - 1);      // Minus 1 to remove the trailing slash.
    }
}