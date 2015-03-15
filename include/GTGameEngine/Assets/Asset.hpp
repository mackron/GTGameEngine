// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_Asset_hpp_
#define __GT_Asset_hpp_

#include <GTLib/String.hpp>
#include <GTLib/ReferenceCountedObject.hpp>
#include "AssetTypes.hpp"

namespace GT
{
    class FileSystem;

    /// Class representing an asset (texture, model, sound, etc.)
    class Asset : public ReferenceCountedObject
    {
    public:

        /// Constructor.
        Asset();

        /// Destructor.
        virtual ~Asset();


        /////////////////////////////////////
        // Virtual Methods
        //
        // Inherited classes must implement these.

        /// Retrieves the class of this asset.
        virtual AssetClass GetClass() const = 0;

        /// Retrieves the type of this asset.
        virtual AssetType GetType() const = 0;

        /// Loads the given file into the asset.
        ///
        /// @param absolutePath [in] The absolute path of the file to load.
        /// @param fileSystem   [in] A reference to the file system object to load the file from.
        ///
        /// @remarks
        ///     This will replace the existing content of the asset.
        virtual bool Load(const char* absolutePath, FileSystem &fileSystem) = 0;
    };
}

#endif