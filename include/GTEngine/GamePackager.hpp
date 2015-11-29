// Copyright (C) 2011 - 2014 David Reid. See included LICENCE file.

#ifndef __GTEngine_GamePackager_hpp_
#define __GTEngine_GamePackager_hpp_

#include <GTEngine/Core/String.hpp>
#include <GTEngine/Core/Vector.hpp>

namespace GTEngine
{
    /// Class used for packaging a game for distribution.
    ///
    /// The packager depends on certain operations being performed in certain orders:
    ///     1) Copy over data directories.
    ///     2) Copy over any other individual files.
    ///     3) Copy over the main executable.
    ///     4) Write the config file.
    class GamePackager
    {
    public:

        /// Constructor.
        ///
        /// @param outputDirectoryAbsolutePath [in] The output directory.
        GamePackager(const char* outputDirectoryAbsolutePath);

        /// Destructor.
        ~GamePackager();


        /// Copies the given data directory over.
        ///
        /// @param sourceAbsolutePath [in] The absolute path of the source directory.
        /// @param destRelativePath   [in] The destination path, relative to the output directory.
        ///
        /// @remarks
        ///     This is recursive.
        ///     @par
        ///     For the root data directory, 'destinationRelativePath' should be nullptr. This is because the
        ///     root data directories will be used when writing the config script.
        ///     @par
        ///     When the directory is a root data directory, it will never copy over the "var" directory.
        void CopyDataDirectory(const char* sourceAbsolutePath, const char* destinationRelativePath = nullptr);

        /// Copies over the executable.
        ///
        /// @param sourceAbsolutePath      [in] The absolute path of the source file.
        /// @param destinationRelativePath [in] The relative path of the destination file.
        bool CopyExecutable(const char* sourceAbsolutePath, const char* destinationRelativePath = nullptr);

        /// Copies over an individual file.
        ///
        /// @param sourceAbsolutePath      [in] The absolute path of the source file.
        /// @param destinationRelativePath [in] The relative path of the destination file.
        ///
        /// @remarks
        ///     When copying the executable, use CopyExecutable() instead.
        bool CopyFile(const char* sourceAbsolutePath, const char* destinationRelativePath = nullptr);


        /// Writes the config file.
        ///
        /// @remarks
        ///     This must be done after copying over the data directories and executable.
        bool WriteConfig();



    private:

        /// The absolute path of the output directory.
        GTLib::String outputDirectoryAbsolutePath;

        /// The paths of data directories relative to 'outputDirectoryAbsolutePath'. These will be used when writing the config file.
        GTLib::Vector<GTLib::String> dataDirectoryRelativePaths;

        /// The relative path of the executable.
        GTLib::String executableRelativePath;
    };
}

#endif