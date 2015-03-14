// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_ImageAsset_hpp_
#define __GT_ImageAsset_hpp_

#include "Asset.hpp"

namespace GT
{
    // Base class for image assets.
    class ImageAsset : public Asset
    {
    public:

        /// Constructor.
        ImageAsset();

        /// Destructor.
        virtual ~ImageAsset();


        /// @copydoc Asset::GetClass()
        AssetClass GetClass() const;
    };
}

#endif
