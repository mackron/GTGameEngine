// Copyright (C) 2011 - 2014 David Reid. See included LICENCE.

#ifndef __GTEngine_IO_hpp_
#define __GTEngine_IO_hpp_

#include <GTLib/IO.hpp>
#include <GTEngine/AssetTypes.hpp>

namespace GTEngine
{
    namespace IO
    {
        /// Determines whether or not the extension of the given file path is a supported image file.
        ///
        /// @param fileName [in] The file name to check.
        ///
        /// @return True if the file is a supported image file.
        bool IsSupportedImageExtension(const char* fileName);

        /// Determines whether or not the extension of the given file path is a supported model file.
        ///
        /// @param fileName [in] The file name to check.
        ///
        /// @return True if the file is a supported model file.
        bool IsSupportedModelExtension(const char* fileName);

        /// Determines whether or not the extension of the given file path is a supported material file.
        ///
        /// @param fileName [in] The file name to check.
        ///
        /// @return True if the file is a supported material file.
        bool IsSupportedMaterialExtension(const char* fileName);

        /// Determines whether or not the extension of the given file path is a supported sound file.
        ///
        /// @param fileName [in] The file name to check.
        ///
        /// @return True if the file is a supported sound file.
        bool IsSupportedSoundExtension(const char* fileName);

        /// Determines whether or not the extension of the given file path is a supported particle effect file.
        ///
        /// @param fileName [in] The file name to check.
        ///
        /// @return True if the file is a supported particle file.
        bool IsSupportedParticleSystemExtension(const char* fileName);

        /// Determines whether or not the extension of the given file path is a supported scene file.
        ///
        /// @param fileName [in] The file name to check.
        ///
        /// @return True if the file is a supported scene file.
        bool IsSupportedSceneExtension(const char* fileName);

        /// Determines whether or not the extension of the given file path is a supported prefab file.
        ///
        /// @param fileName [in] The file name to check.
        ///
        /// @return True if the file is a supported prefab file.
        bool IsSupportedPrefabExtension(const char* fileName);

        /// Determines whether or not the extension of the given file path is a supported script file.
        ///
        /// @param fileName [in] The file name to check.
        ///
        /// @return True if the file is a supported script file.
        bool IsSupportedScriptExtension(const char* fileName);



        /// Retrieves the type of asset based on the extension of the given file name.
        ///
        /// @param fileName [in] The file name whose asset type is being determined.
        ///
        /// @return The asset type for the given file name.
        ///
        /// @remarks
        ///     If the asset type is unknown, it will assume a text file.
        AssetType GetAssetTypeFromExtension(const char* fileName);



        /// Retrieves the base path of the given absolute and relative paths.
        ///
        /// @param absolutePath [in] The absolute path.
        /// @param relativePath [in] The relative path.
        ///
        /// @remarks
        ///     The relative path can not contain '.' and '..' characters.
        GTLib::String GetBasePath(const char* absolutePath, const char* relativePath);
    }
}

#endif
