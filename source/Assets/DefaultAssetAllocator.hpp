// Copyright (C) 2011 - 2015 David Reid. See included LICENCE file.

#ifndef __GT_DefaultAssetAllocator_hpp_
#define __GT_DefaultAssetAllocator_hpp_

#include <GTGameEngine/Config.hpp>

#if defined(GT_BUILD_DEFAULT_ASSETS)
#include <GTGameEngine/Assets/AssetAllocator.hpp>

namespace GT
{
    /// The default asset allocator for assets that are supported natively by the engine.
    class DefaultAssetAllocator : public AssetAllocator
    {
    public:
        
        /// @copydoc AssetAllocator::IsAssetTypeSupported()
        bool IsAssetTypeSupported(AssetType assetType) const;

        /// @copydoc AssetAllocator::CreateAsset()
        Asset* CreateAsset(AssetType assetType);

        /// @copydoc AssetAllocator::DeleteAsset()
        void DeleteAsset(Asset* pAsset);
    };
}

#endif

#endif
