// Copyright (C) 2011 - 2013 David Reid. See included LICENCE file or GTEngine.hpp.

#include <GTEngine/IO.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Audio/AudioComposer.hpp>
#include <GTImage/Loader.hpp>
#include <GTLib/Path.hpp>
#include <GTLib/Strings/Find.hpp>

namespace GTEngine
{
    namespace IO
    {
        bool IsSupportedImageExtension(const char* fileName)
        {
            return GTImage::Loader::IsExtensionSupported(GTLib::Path::Extension(fileName));
        }

        bool IsSupportedModelExtension(const char* fileName)
        {
            return ModelLibrary::IsExtensionSupported(GTLib::Path::Extension(fileName));
        }

        bool IsSupportedMaterialExtension(const char* fileName)
        {
            auto extension = GTLib::Path::Extension(fileName);

            return GTLib::Strings::Equal<false>(extension, "material") ||
                   GTLib::Strings::Equal<false>(extension, "gtmaterial");
        }

        bool IsSupportedSoundExtension(const char* fileName)
        {
            return AudioComposer::IsFileExtensionSupported(GTLib::Path::Extension(fileName));
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



        GTLib::String GetBasePath(const char* absolutePath, const char* relativePath)
        {
            // All we're going to do is remove the 'relativePath' part from the absolute path.
            const char* start = absolutePath;
            const char* end   = GTLib::Strings::FindFirst(absolutePath, relativePath);

            return GTLib::String(start, end - start - 1);      // Minus 1 to remove the trailing slash.
        }
    }
}