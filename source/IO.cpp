
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
            return GTCore::Strings::Equal<false>(GTCore::Path::Extension(fileName), "material");
        }

        bool IsSupportedSoundExtension(const char* fileName)
        {
            return AudioComposer::IsExtensionSupported(GTCore::Path::Extension(fileName));
        }

        bool IsSupportedParticleEffectExtension(const char* fileName)
        {
            return GTCore::Strings::Equal<false>(GTCore::Path::Extension(fileName), "gtparticle");
        }

        bool IsSupportedSceneExtension(const char* fileName)
        {
            return GTCore::Strings::Equal<false>(GTCore::Path::Extension(fileName), "gtscene");
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
            else if (IsSupportedParticleEffectExtension(fileName))
            {
                return AssetType_ParticleEffect;
            }
            else if (IsSupportedSceneExtension(fileName))
            {
                return AssetType_Scene;
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