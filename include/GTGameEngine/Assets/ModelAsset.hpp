// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_ModelAsset_hpp_
#define __GT_ModelAsset_hpp_

#include "Asset.hpp"

namespace GT
{
    /// Base class for model assets.
    class ModelAsset : public Asset
    {
    public:

        /// Constructor.
        ModelAsset(AssetType assetType);

        /// Destructor.
        virtual ~ModelAsset();


        /// @copydoc Asset::GetClass()
        AssetClass GetClass() const;


    };
}

#endif
