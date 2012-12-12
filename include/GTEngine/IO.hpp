
#ifndef __GTEngine_IO_hpp_
#define __GTEngine_IO_hpp_

#include <GTCore/IO.hpp>

namespace GTEngine
{
    namespace IO
    {
        /// Determines whether or not the extension of the given file path is a supported model file.
        ///
        /// @param fileName [in] The file name to check.
        ///
        /// @return True if the file is a supported model file.
        bool IsSupportedModelExtension(const char* fileName);

        /// Determines whether or not the extension of the given file path is a supported image file.
        ///
        /// @param fileName [in] The file name to check.
        ///
        /// @return True if the file is a support image file.
        bool IsSupportedImageExtension(const char* fileName);

        /// Determines whether or not the extension of the given file path is a supported sound file.
        ///
        /// @param fileName [in] The file name to check.
        ///
        /// @return True if the file is a supported sound file.
        bool IsSupportedSoundExtension(const char* fileName);
    }
}


#endif