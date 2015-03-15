// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_ImageAsset_hpp_
#define __GT_ImageAsset_hpp_

#include "Asset.hpp"
#include "../Rendering/RenderingTypes.hpp"

namespace GT
{
    // Base class for image assets.
    class ImageAsset : public Asset
    {
    public:

        /// Constructor.
        ImageAsset(AssetType type);

        /// Destructor.
        virtual ~ImageAsset();


        /// @copydoc Asset::GetClass()
        AssetClass GetClass() const;


        /// Retrieves the width of the image.
        ///
        /// @return The width of the image.
        virtual unsigned int GetImageWidth() const = 0;

        /// Retrieves the height of the image.
        ///
        /// @return The height of the image.
        virtual unsigned int GetImageHeight() const = 0;

        /// Retrieves the format of the image.
        ///
        /// @return The format of the image.
        virtual TextureFormat GetImageFormat() const = 0;

        /// Retrieves a pointer to the data of the image.
        ///
        /// @return A pointer to the image data.
        virtual void* GetImageData() const = 0;
    };
}

#endif
