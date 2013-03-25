// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/IO.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Audio/AudioComposer.hpp>
#include <GTImage/Loader.hpp>
#include <GTCore/Path.hpp>
#include <GTCore/Strings/Find.hpp>

namespace GTEngine
{
    namespace IO
    {
        bool IsSupportedImageExtension(const char* fileName)
        {
            return GTImage::Loader::IsExtensionSupported(GTCore::Path::Extension(fileName));
        }

        bool IsSupportedModelExtension(const char* fileName)
        {
            return ModelLibrary::IsExtensionSupported(GTCore::Path::Extension(fileName));
        }

        bool IsSupportedMaterialExtension(const char* fileName)
        {
            auto extension = GTCore::Path::Extension(fileName);

            return GTCore::Strings::Equal<false>(extension, "material") ||
                   GTCore::Strings::Equal<false>(extension, "gtmaterial");
        }

        bool IsSupportedSoundExtension(const char* fileName)
        {
            return AudioComposer::IsExtensionSupported(GTCore::Path::Extension(fileName));
        }

        bool IsSupportedParticleSystemExtension(const char* fileName)
        {
            return GTCore::Strings::Equal<false>(GTCore::Path::Extension(fileName), "gtparticle");
        }

        bool IsSupportedSceneExtension(const char* fileName)
        {
            return GTCore::Strings::Equal<false>(GTCore::Path::Extension(fileName), "gtscene");
        }

        bool IsSupportedPrefabExtension(const char* fileName)
        {
            return GTCore::Strings::Equal<false>(GTCore::Path::Extension(fileName), "gtprefab");
        }

        bool IsSupportedScriptExtension(const char* fileName)
        {
            auto extension = GTCore::Path::Extension(fileName);

            return GTCore::Strings::Equal<false>(extension, "gtscript") ||
                   GTCore::Strings::Equal<false>(extension, "script")   ||
                   GTCore::Strings::Equal<false>(extension, "lua");
        }



        AssetType GetAssetTypeFromExtension(const char* fileName)
        {
            if (IsSupportedImageExtension(fileName))
            {
                return AssetType_Image;
            }
            else if (IsSupportedModelExtension(fileName))
            {
                return AssetType_Model;
            }
            else if (IsSupportedMaterialExtension(fileName))
            {
                return AssetType_Material;
            }
            else if (IsSupportedSoundExtension(fileName))
            {
                return AssetType_Sound;
            }
            else if (IsSupportedParticleSystemExtension(fileName))
            {
                return AssetType_ParticleSystem;
            }
            else if (IsSupportedSceneExtension(fileName))
            {
                return AssetType_Scene;
            }
            else if (IsSupportedPrefabExtension(fileName))
            {
                return AssetType_Prefab;
            }
            else if (IsSupportedScriptExtension(fileName))
            {
                return AssetType_Script;
            }
            else
            {
                return AssetType_TextFile;
            }
        }



        GTCore::String GetBasePath(const char* absolutePath, const char* relativePath)
        {
            // All we're going to do is remove the 'relativePath' part from the absolute path.
            const char* start = absolutePath;
            const char* end   = GTCore::Strings::FindFirst(absolutePath, relativePath);

            return GTCore::String(start, end - start - 1);      // Minus 1 to remove the trailing slash.
        }
    }
}