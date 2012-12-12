
#include <GTEngine/IO.hpp>
#include <GTEngine/ModelLibrary.hpp>
#include <GTEngine/Audio/AudioComposer.hpp>
#include <GTImage/Loader.hpp>
#include <GTCore/Path.hpp>

namespace GTEngine
{
    namespace IO
    {
        bool IsSupportedModelExtension(const char* fileName)
        {
            return ModelLibrary::IsExtensionSupported(GTCore::Path::Extension(fileName));
        }

        bool IsSupportedImageExtension(const char* fileName)
        {
            return GTImage::Loader::IsExtensionSupported(GTCore::Path::Extension(fileName));
        }

        bool IsSupportedSoundExtension(const char* fileName)
        {
            return AudioComposer::IsExtensionSupported(GTCore::Path::Extension(fileName));
        }
    }
}