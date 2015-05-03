// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_MaterialAsset_hpp_
#define __GT_MaterialAsset_hpp_

#include "Asset.hpp"

namespace GT
{
    /// Base class for material assets.
    class MaterialAsset : public Asset
    {
    public:

        /// Constructor.
        MaterialAsset(AssetType assetType);

        /// Destructor.
        virtual ~MaterialAsset();


        /// @copydoc Asset::GetClass()
        AssetClass GetClass() const;
    };
}

#endif
