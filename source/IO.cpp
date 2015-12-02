// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#include <GTGE/IO.hpp>
#include <GTGE/ModelLibrary.hpp>
#include <GTGE/Core/ImageLoader.hpp>
#include <GTGE/Core/Strings/Find.hpp>
#include <easy_path/easy_path.h>

namespace GT
{
    bool IsSupportedImageExtension(const char* fileName)
    {
        return ImageLoader::IsExtensionSupported(easypath_extension(fileName));
    }

    bool IsSupportedModelExtension(const char* fileName)
    {
        return ModelLibrary::IsExtensionSupported(easypath_extension(fileName));
    }

    bool IsSupportedMaterialExtension(const char* fileName)
    {
        const char* extension = easypath_extension(fileName);

        return Strings::Equal<false>(extension, "material") ||
               Strings::Equal<false>(extension, "gtmaterial");
    }

    bool IsSupportedSoundExtension(const char* fileName)
    {
        const char* extension = easypath_extension(fileName);
            
        return Strings::Equal<false>(extension, "wav") ||
               Strings::Equal<false>(extension, "ogg");
    }

    bool IsSupportedParticleSystemExtension(const char* fileName)
    {
        return Strings::Equal<false>(easypath_extension(fileName), "gtparticle");
    }

    bool IsSupportedSceneExtension(const char* fileName)
    {
        return Strings::Equal<false>(easypath_extension(fileName), "gtscene");
    }

    bool IsSupportedPrefabExtension(const char* fileName)
    {
        return Strings::Equal<false>(easypath_extension(fileName), "gtprefab");
    }

    bool IsSupportedScriptExtension(const char* fileName)
    {
        auto extension = easypath_extension(fileName);

        return Strings::Equal<false>(extension, "gtscript") ||
               Strings::Equal<false>(extension, "script")   ||
               Strings::Equal<false>(extension, "lua");
    }



    AssetType GetAssetClassFromExtension(const char* fileName)
    {
        // If the path is a directory instead of a file, we need to return AssetType_None.
        if (Strings::IsNullOrEmpty(easypath_file_name(fileName)))
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



    String GetBasePath(const char* absolutePath, const char* relativePath)
    {
        // All we're going to do is remove the 'relativePath' part from the absolute path.
        const char* start = absolutePath;
        const char* end   = Strings::FindFirst(absolutePath, relativePath);

        return String(start, end - start - 1);      // Minus 1 to remove the trailing slash.
    }
}